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

			//Crea internamente el FGameplayEffectSpecHandle por lo que no nos permite pasar parametros adicionales, más simple y limpio pero más limitado.
			/*
			const UGameplayEffect* GE = Effect->GetDefaultObject<UGameplayEffect>();
			if (GE)
			{
				
				ApplyGameplayEffectToSelf(GE, 1.0f, MakeEffectContext());
			}
			*/
		}
	}
	/*
	if(StartUpAbilitiesGiven)
	{
		for (const TSubclassOf<UGameplayAbility>& Ability : Abilities)
		{
			if (Ability)
			{
				GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
			}
		}
		StartUpAbilitiesGiven = true;
	}
	*/
}

void UHAbilitySystemComponent::GiveInitialAbilities()
{
	if(!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
/*
	for (const TSubclassOf<UGameplayAbility>& Ability : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(Ability,0,-1,nullptr));
	}

	for (const TSubclassOf<UGameplayAbility>& Ability : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(Ability,0,-1,nullptr));
	}
*/
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
