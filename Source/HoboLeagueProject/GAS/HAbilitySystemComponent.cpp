// Fill out your copyright notice in the Description page of Project Settings.


#include "HAbilitySystemComponent.h"

#include "FGameplayTags.h"
#include "HAttributeSet.h"


UHAbilitySystemComponent::UHAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	GetGameplayAttributeValueChangeDelegate(UHAttributeSet::GetHealthAttribute()).AddUObject(
		this, &UHAbilitySystemComponent::HealthUpdated);
	
	GetGameplayAttributeValueChangeDelegate(UHAttributeSet::GetStaminaAttribute()).AddUObject(
		this, &UHAbilitySystemComponent::StaminaUpdated);
	
	GetGameplayAttributeValueChangeDelegate(UHAttributeSet::GetOverdoseAttribute()).AddUObject(
		this, &UHAbilitySystemComponent::OverdoseUpdated);
}

void UHAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TSubclassOf<UGameplayEffect>& Effect : InitialEffects)
	{
		if (Effect)
		{
			//Permite la creación de un FGameplayeffectSpectHandle por lo que podemos pasarle parametros.
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(Effect, 1, MakeEffectContext());
			ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
	}
}

void UHAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	//Vincular las habilidades al input mediante un mapa
	for (const TPair<EHAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0.0f, (int32)AbilityPair.Key, nullptr));
	}

	for (const TPair<EHAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1.0f, (int32)AbilityPair.Key, nullptr));
	}

	for (TSubclassOf<UGameplayAbility>& Ability : PassiveAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(Ability, 1.0f, INDEX_NONE, nullptr));
	}
}

FGameplayAbilitySpecHandle UHAbilitySystemComponent::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass,
                                                                  int32 Level)
{
	if (!AbilityClass || !GetOwner() || !GetOwner()->HasAuthority()) return FGameplayAbilitySpecHandle();

	return GiveAbility(FGameplayAbilitySpec(AbilityClass, Level, INDEX_NONE, this));
}

void UHAbilitySystemComponent::RemoveAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass || !GetOwner() || !GetOwner()->HasAuthority()) return;

	if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityClass))
	{
		ClearAbility(Spec->Handle);
	}
}

void UHAbilitySystemComponent::RemoveAllGrantedAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (auto It = ActivatableAbilities.Items.CreateIterator(); It; ++It)
	{
		ClearAbility(It->Handle);
	}
}

void UHAbilitySystemComponent::BindAbilityToInputID(EHAbilityInputID InputID,
                                                    TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass || !GetOwner() || !GetOwner()->HasAuthority()) return;

	if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityClass))
	{
		Spec->InputID = static_cast<int32>(InputID);
		MarkAbilitySpecDirty(*Spec);
	}
}

void UHAbilitySystemComponent::UnbindAbilityByInputID_Class(EHAbilityInputID InputID,
                                                            TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass || !GetOwner() || !GetOwner()->HasAuthority()) return;

	// Find the ability spec for this ability
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityClass);
	if (Spec)
	{
		// If the input matches, unbind it
		if (Spec->InputID == static_cast<int32>(InputID))
		{
			Spec->InputID = INDEX_NONE; // No input will trigger this ability now
			MarkAbilitySpecDirty(*Spec); // Replicate change to clients
		}
	}
}

void UHAbilitySystemComponent::UnbindAllAbilitiesFromInputID(EHAbilityInputID InputID)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	const int32 TargetID = static_cast<int32>(InputID);

	// Iterate over all granted abilities
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == TargetID)
		{
			Spec.InputID = INDEX_NONE; // Clear the binding
			MarkAbilitySpecDirty(Spec); // Ensure replication to clients
		}
	}
}

void UHAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (!GetOwner()) return;
	if (OnAttributeChangeData.NewValue <= 0 && GetOwner()->HasAuthority() && DeathEffect)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(DeathEffect, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UHAbilitySystemComponent::StaminaUpdated(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (!GetOwner()) return;

	const float MaxStamina = GetSet<UHAttributeSet>()->GetMaxStamina();

	if (GetOwner()->HasAuthority())
	{
		if (OnAttributeChangeData.NewValue < MaxStamina &&
			!HasMatchingGameplayTag(FHGameplayTags::Get().Status_StaminaDepletion))
		{
			// This is the valid GAS method to add a replicated tag
			AddLooseGameplayTag(FHGameplayTags::Get().Status_StaminaDepletion);
		}
		else if (OnAttributeChangeData.NewValue >= MaxStamina &&
				 HasMatchingGameplayTag(FHGameplayTags::Get().Status_StaminaDepletion))
		{
			RemoveLooseGameplayTag(FHGameplayTags::Get().Status_StaminaDepletion);
		}
	}
}


void UHAbilitySystemComponent::OverdoseUpdated(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (!GetOwner()) return;

	const float MaxOverdose = GetSet<UHAttributeSet>()->GetMaxOverdose();

	if (OnAttributeChangeData.NewValue >= MaxOverdose && GetOwner()->HasAuthority() && OverdoseEffect)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(OverdoseEffect, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}
