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

void UHAbilitySystemComponent::GrantItemAbility(EHAbilityInputID InputID, TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass || !GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == static_cast<int32>(InputID))
		{
			Spec.InputID = INDEX_NONE; // Prevent it from triggering
			MarkAbilitySpecDirty(Spec); // Replicate change to clients
		}
	}
	
	GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, static_cast<int32>(InputID), this));
}

void UHAbilitySystemComponent::UnBindAbility(EHAbilityInputID InputID, TSubclassOf<UGameplayAbility> AbilityClass)
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

