// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SwapItem.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/Item/HItemType.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItemDataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumable.h"
#include "HoboLeagueProject/Item/PlayerItem/Weapon/HWeapon.h"

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

	const TArray<AHPlayerItem*>& Items = Inventory->GetInventoryItems();
	AHPlayerItem* CurrentlyActive = (ItemTypeToSwap == EItemType::Weapon) ? 
									 Cast<AHPlayerItem>(Inventory->GetActiveWeapon()) : 
									 Cast<AHPlayerItem>(Inventory->GetActiveConsumable());
	
	int32 CurrentIndex = Items.IndexOfByKey(CurrentlyActive);
	
	for (int32 i = 1; i <= Items.Num(); ++i)
	{
		int32 NextIndex = (CurrentIndex + i) % Items.Num();
		AHPlayerItem* PotentialItem = Items[NextIndex];

		if (PotentialItem && PotentialItem->ItemData->GetItemType() == ItemTypeToSwap)
		{
			if (PotentialItem != CurrentlyActive)
			{
				Inventory->EquipItem(PotentialItem);
				break; 
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
