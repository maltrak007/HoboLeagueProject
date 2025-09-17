// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SwapItem.h"

#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"

UGA_SwapItem::UGA_SwapItem()
{
}

void UGA_SwapItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerCharacter* PC = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!PC) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UHInventoryComponent* Inventory = PC->FindComponentByClass<UHInventoryComponent>();
	if (!Inventory)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	Inventory->EquipItem(ItemTypeToSwap);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
