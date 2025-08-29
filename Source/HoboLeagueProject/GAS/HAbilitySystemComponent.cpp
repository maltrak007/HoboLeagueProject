// Fill out your copyright notice in the Description page of Project Settings.


#include "HAbilitySystemComponent.h"


UHAbilitySystemComponent::UHAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHAbilitySystemComponent::ApplyInitialEffects()
{
	if(!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (const TSubclassOf<UGameplayEffect>& Effect : InitialEffects)
	{
		if (Effect)
		{
			//Permite la creación de un FGameplayeffectSpectHandle por lo que podemos pasarle parametros.
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(Effect,1,MakeEffectContext());
			ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
	}
}

void UHAbilitySystemComponent::GiveInitialAbilities()
{
	if(!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	//Vincular las habilidades al input mediante un mapa
	for(const TPair <EHAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0.0f, (int32)AbilityPair.Key,nullptr));
	}

	for(const TPair <EHAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1.0f, (int32)AbilityPair.Key,nullptr));
	}
}

// void UHAbilitySystemComponent::GrantAndBindItemAbilityToInputID(EHAbilityInputID InputID, TSubclassOf<UGameplayAbility> AbilityClass)
// {
// 	if (!AbilityClass || !GetOwner() || !GetOwner()->HasAuthority()) return;
// 	
// 	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
// 	{
// 		if (Spec.InputID == static_cast<int32>(InputID))
// 		{
// 			Spec.InputID = INDEX_NONE; // Prevent it from triggering
// 			MarkAbilitySpecDirty(Spec); // Replicate change to clients
// 		}
// 	}
// 	
// 	GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, static_cast<int32>(InputID), this));
// }
//
// void UHAbilitySystemComponent::RemoveAbilityByInputID(EHAbilityInputID InputID)
// {
// 	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
//
// 	// Loop through all granted abilities and clear the one with the matching InputID
// 	for (auto It = ActivatableAbilities.Items.CreateIterator(); It; ++It)
// 	{
// 		if (It->InputID == static_cast<int32>(InputID))
// 		{
// 			ClearAbility(It->Handle); // Removes the ability
// 			return; // Exit after clearing first match
// 		}
// 	}
// }
FGameplayAbilitySpecHandle UHAbilitySystemComponent::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
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

void UHAbilitySystemComponent::BindAbilityToInputID(EHAbilityInputID InputID, TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass || !GetOwner() || !GetOwner()->HasAuthority()) return;
	
	if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityClass))
	{
		Spec->InputID = static_cast<int32>(InputID);
		MarkAbilitySpecDirty(*Spec);
	}
}

void UHAbilitySystemComponent::UnbindAbilityByInputID(EHAbilityInputID InputID, TSubclassOf<UGameplayAbility> AbilityClass)
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




