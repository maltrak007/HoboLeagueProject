// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SwapItem.h"

#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/Item/HItemType.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItemDataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumable.h"
#include "HoboLeagueProject/Item/PlayerItem/Weapon/HWeapon.h"

UGA_SwapItem::UGA_SwapItem()
{
}

void UGA_SwapItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

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

	// Look at the Type to retrieve the correct item
	if (ItemTypeToSwap == EItemType::Weapon)
	{
		for (auto Element : Inventory->GetInventoryItems())
		{
			if (Element && Element->ItemData->GetItemType() == EItemType::Weapon && Cast<AHWeapon>(Element) != Inventory->GetActiveWeapon())
			{
				Inventory->EquipItem(Element);
				break;
			}
		}
	}
	else if (ItemTypeToSwap == EItemType::Consumable)
	{
		for (auto Element : Inventory->GetInventoryItems())
		{
			if (Element && Element->ItemData->GetItemType() == EItemType::Consumable && Cast<AHConsumable>(Element) != Inventory->GetActiveConsumable())
			{
				Inventory->EquipItem(Element);
				break;
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
