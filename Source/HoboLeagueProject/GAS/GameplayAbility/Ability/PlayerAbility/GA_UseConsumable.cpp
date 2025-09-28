// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_UseConsumable.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/ConsumableDataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumable.h"

UGA_UseConsumable::UGA_UseConsumable()
{
}

void UGA_UseConsumable::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, NAME_None, ConsumableMontage);
		PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_UseConsumable::K2_EndAbility);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_UseConsumable::K2_EndAbility);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_UseConsumable::K2_EndAbility);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_UseConsumable::K2_EndAbility);
		PlayMontageTask->ReadyForActivation();
	}

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
			GetUseConsumableEventTag());
		WaitTargetingEventTask->EventReceived.AddDynamic(this, &UGA_UseConsumable::ApplyConsumableEffect);
		WaitTargetingEventTask->ReadyForActivation();
	}
}

void UGA_UseConsumable::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                   bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FGameplayTag UGA_UseConsumable::GetUseConsumableEventTag()
{
	return FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Consume));
}

void UGA_UseConsumable::ApplyConsumableEffect(FGameplayEventData Data)
{
	if (!K2_HasAuthority())
		return;

	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	
	APlayerCharacter* PC = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	
	AHConsumable* ConsumableItem = Cast<AHConsumable>(PC->GetInventoryComponent()->GetEquippedItem());

	if (!ConsumableItem)
		return;

	UConsumableDataAsset* ItemConsumableDataAsset = ConsumableItem->GetItemConsumableDataAsset();

	if (!ItemConsumableDataAsset)
		return;

	if (ItemConsumableDataAsset->GetHasManyGameplayEffects())
	{
		for (auto Element : ItemConsumableDataAsset->GetGameplayEffects())
		{
			const int32 AbilityLevel = GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Element, AbilityLevel);

			if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
			{
				PC->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		ConsumableItem->ReduceConsumableCharges();
		ConsumableItem->OnConsumableUsed.Broadcast();
	}
	else
	{
		const int32 AbilityLevel = GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
			ItemConsumableDataAsset->GetGameplayEffect(), AbilityLevel);

		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			PC->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		ConsumableItem->ReduceConsumableCharges();
		ConsumableItem->OnConsumableUsed.Broadcast();
	}
}
