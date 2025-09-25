// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_RegenerateStamina.h"
#include "AbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"


UGA_RegenerateStamina::UGA_RegenerateStamina()
{

}

void UGA_RegenerateStamina::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (ActorInfo->AbilitySystemComponent.IsValid())
	{
		FGameplayEffectSpecHandle RegenSpec = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(
			StaminaRegenerationEffect, 1.f, ActorInfo->AbilitySystemComponent->MakeEffectContext());

		if (RegenSpec.IsValid())
		{
			ActiveStaminaDrain = ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*RegenSpec.Data.Get());
		}
	}
}

void UGA_RegenerateStamina::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->AbilitySystemComponent.IsValid() && ActiveStaminaDrain.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveStaminaDrain);
		ActiveStaminaDrain.Invalidate();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
