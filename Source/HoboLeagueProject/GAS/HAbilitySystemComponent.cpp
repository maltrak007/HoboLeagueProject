// Fill out your copyright notice in the Description page of Project Settings.


#include "HAbilitySystemComponent.h"

#include "FGameplayTags.h"
#include "HAttributeSet.h"
#include "GameplayAbility/HBaseGameplayAbility.h"


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

TArray<FGameplayAbilitySpecHandle> UHAbilitySystemComponent::GrantAbility(
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant, int32 Level)
{
	if (!GetOwner()->HasAuthority()) return TArray<FGameplayAbilitySpecHandle>();

	TArray<FGameplayAbilitySpecHandle> GrantedHandles;
	
	for (TSubclassOf<UGameplayAbility> Ability : AbilitiesToGrant)
	{
		int32 InputID = -1;
		if (const UHBaseGameplayAbility* BaseAbilityCDO = GetDefault<UHBaseGameplayAbility>(Ability))
		{
			InputID = static_cast<int32>(BaseAbilityCDO->AbilityInputID);
		}
		
		FGameplayAbilitySpecHandle SpecHandle = GiveAbility(FGameplayAbilitySpec(Ability, Level, InputID, nullptr));
		GrantedHandles.Add(SpecHandle);
	}
	
	return GrantedHandles;
}

void UHAbilitySystemComponent::RemoveGrantedAbility(TArray<FGameplayAbilitySpecHandle> HandleRemoval)
{
	if (!GetOwner()->HasAuthority()) return;

	for (FGameplayAbilitySpecHandle SpecHandle : HandleRemoval)
	{
		ClearAbility(SpecHandle);
	}
}

void UHAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
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
