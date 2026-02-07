// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_RegenerateStamina.h"
#include "AbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "HoboLeagueProject/GAS/GameplayEffect/GE_StaminaRegeneration.h"


UGA_RegenerateStamina::UGA_RegenerateStamina()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	SetAssetTags(FGameplayTagContainer(FHGameplayTags::Get().Abilities_RegenerateStamina));
	ActivationBlockedTags.AddTag(FHGameplayTags::Get().Status_Dead);
	ActivationBlockedTags.AddTag(FHGameplayTags::Get().Status_Overdosing);
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

	if (ActorInfo->AbilitySystemComponent.IsValid() && ActorInfo->OwnerActor->HasAuthority())
	{
		FGameplayEffectSpecHandle RegenSpec = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(
		StaminaRegenEffectClass, 1.f, ActorInfo->AbilitySystemComponent->MakeEffectContext());
		
		if (RegenSpec.IsValid())
		{
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*RegenSpec.Data.Get());
		}
	}
}

void UGA_RegenerateStamina::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


