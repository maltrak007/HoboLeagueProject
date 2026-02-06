// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryComponent.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/Item/HItemType.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItemDataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumable.h"
#include "HoboLeagueProject/Item/PlayerItem/Weapon/HWeapon.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UHInventoryComponent::UHInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UHInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UHInventoryComponent::AddItem(AHPlayerItem* ItemToAdd)
{
	// Checks if the item is valid
	// Checks if the player HasAuthority()
	if (!ItemToAdd || !GetOwner() || !GetOwner()->HasAuthority() || !AbilitySystemComp) return;

	// Checks if the item is already in the inventory
	if (InventoryItems.Contains(ItemToAdd)) return;

	// Checks if this item type can be added to the inventory (max number of items)
	EItemType ItemType = ItemToAdd->ItemData->GetItemType();

	// If it surpasses the limit remove the oldest item of his type
	int32 TypeCount = 0;
	for (AHPlayerItem* Item : InventoryItems)
	{
		if (Item && Item->ItemData->GetItemType() == ItemType)
		{
			TypeCount++;
		}
	}

	if ((ItemType == EItemType::Weapon && TypeCount >= MaxWeapons) ||
		(ItemType == EItemType::Consumable && TypeCount >= MaxConsumables))
	{
		for (int32 i = InventoryItems.Num() - 1; i >= 0; i--)
		{
			AHPlayerItem* Existing = InventoryItems[i];
			if (Existing && Existing->ItemData->GetItemType() == ItemType)
			{
				RemoveItem(Existing);
				break;
			}
		}
	}

	// Add the item to the inventory
	InventoryItems.Add(ItemToAdd);
	
	// If auto-equip option is true & there is no equipped item of this type equip it, else attach it to the holster socket
	if ((ItemType == EItemType::Weapon && !ActiveWeapon && bShouldAutoEquip) ||
		(ItemType == EItemType::Consumable && !ActiveConsumable && bShouldAutoEquip))
	{
		EquipItem(ItemToAdd);
	}
	else
	{
		ItemToAdd->AttachToHolsterSocket(Cast<APlayerCharacter>(GetOwner()));
	}
}

void UHInventoryComponent::RemoveItem(AHPlayerItem* ItemToRemove)
{
	// Checks if the item is valid
	// Checks if the player HasAuthority()
	if (!ItemToRemove || !GetOwner() || !GetOwner()->HasAuthority() || !AbilitySystemComp) return;

	// If the item to remove is the current item equipped unequip it
	if (Cast<AHWeapon>(ItemToRemove) == ActiveWeapon || Cast<AHConsumable>(ItemToRemove) == ActiveConsumable)
	{
		UnequipItem(ItemToRemove);
	}
	
	// Deattach it from the player
	ItemToRemove->DetachFromPlayer();

	InventoryItems.RemoveSwap(ItemToRemove);
}

void UHInventoryComponent::EquipItem(AHPlayerItem* ItemToEquip)
{
	// Checks if the player HasAuthority()
	//if (!ItemToEquip || !GetOwner() || !GetOwner()->HasAuthority() || !ASC) return;
	if (!GetOwner() || !GetOwner()->HasAuthority() || !AbilitySystemComp) return;
	
	// If there is another item of the same type unequip it -> UnequipItem ()
	// Set the new item as active
	//Bind Input from ASC
	// AttachToActiveSocket
	switch (ItemToEquip->ItemData->GetItemType())
	{
	case EItemType::Weapon:
		{
			if (ActiveWeapon == ItemToEquip)
				return;

			if (ActiveWeapon)
			{
				AbilitySystemComp->RemoveReplicatedLooseGameplayTag(ItemToEquip->ItemData->GetItemTag());
				UnequipItem(ActiveWeapon);
			}

			SetActiveWeapon(Cast<AHWeapon>(ItemToEquip));

			//Grant Item Abilities to the player
			ItemToEquip->GrantedAbilityHandles = AbilitySystemComp->GrantAbility(ItemToEquip->ItemData->GetItemAbilities());
			
			GetActiveWeapon()->AttachToActiveSocket(Cast<APlayerCharacter>(GetOwner()));
			break;
		}
	case EItemType::Consumable:
		{
			if (ActiveConsumable == ItemToEquip)
				return;

			if (ActiveConsumable)
			{
				UnequipItem(ActiveConsumable);
			}

			SetActiveConsumable(Cast<AHConsumable>(ItemToEquip));
			
			ItemToEquip->GrantedAbilityHandles = AbilitySystemComp->GrantAbility(ItemToEquip->ItemData->GetItemAbilities());
			GetActiveConsumable()->AttachToActiveSocket(Cast<APlayerCharacter>(GetOwner()));
			break;
		}

	default:
		break;
	}
}

void UHInventoryComponent::UnequipItem(AHPlayerItem* ItemToUnequip)
{
	// Checks if the item is valid
	// Checks if the player HasAuthority()
	if (!ItemToUnequip || !GetOwner() || !GetOwner()->HasAuthority() || !AbilitySystemComp) return;
	
	AbilitySystemComp->RemoveGrantedAbility(ItemToUnequip->GrantedAbilityHandles);
	
	ItemToUnequip->AttachToHolsterSocket(Cast<APlayerCharacter>(GetOwner()));

	EItemType ItemType = ItemToUnequip->ItemData ? ItemToUnequip->ItemData->GetItemType() : EItemType::Other;

	if (ItemType == EItemType::Weapon)
		SetActiveWeapon(nullptr);
	else if (ItemType == EItemType::Consumable)
		SetActiveConsumable(nullptr);
}

void UHInventoryComponent::OnRep_ActiveWeapon()
{
	if (ActiveWeapon)
	{
		ActiveWeapon->AttachToActiveSocket(Cast<APlayerCharacter>(GetOwner()));
	}
}

void UHInventoryComponent::OnRep_ActiveConsumable()
{
	if (ActiveConsumable)
	{
		ActiveConsumable->AttachToActiveSocket(Cast<APlayerCharacter>(GetOwner()));
	}
}

void UHInventoryComponent::LinkAbilitySystemComponent(UAbilitySystemComponent* ASC)
{
	AbilitySystemComp = Cast<UHAbilitySystemComponent>(ASC);
}


void UHInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHInventoryComponent, InventoryItems);
	DOREPLIFETIME(UHInventoryComponent, ActiveWeapon);
	DOREPLIFETIME(UHInventoryComponent, ActiveConsumable);
}
