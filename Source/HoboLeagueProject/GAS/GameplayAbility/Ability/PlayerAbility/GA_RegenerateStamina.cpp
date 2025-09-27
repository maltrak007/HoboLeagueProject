// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_RegenerateStamina.h"
#include "AbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "HoboLeagueProject/GAS/GameplayEffect/GE_StaminaRegeneration.h"


UGA_RegenerateStamina::UGA_RegenerateStamina()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	AbilityTags.AddTag(FHGameplayTags::Get().Abilities_RegenerateStamina);
	ActivationBlockedTags.AddTag(FHGameplayTags::Get().Status_Dead);
	ActivationBlockedTags.AddTag(FHGameplayTags::Get().Status_Overdosing);
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FHGameplayTags::Get().Status_StaminaDepletion;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::OwnedTagPresent;
	AbilityTriggers.Add(TriggerData);
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
	UGE_StaminaRegeneration::StaticClass(), 1.f, ActorInfo->AbilitySystemComponent->MakeEffectContext());

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
	if (ActorInfo->AbilitySystemComponent.IsValid() && ActiveStaminaRegeneration.IsValid())
	{
		// Remove the active effect
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveStaminaRegeneration);
		ActiveStaminaRegeneration.Invalidate();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
