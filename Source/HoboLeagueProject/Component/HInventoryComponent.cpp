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

	if ((ItemType == EItemType::Weapon && TypeCount >= numMaxWeapons) ||
		(ItemType == EItemType::Consumable && TypeCount >= numMaxConsumables))
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

	// Grant the item abilities to the ASC
	if (ItemType == EItemType::Weapon)
	{
		AbilitySystemComp->GrantAbility(ItemToAdd->ItemData->GetItemPrimaryAbility(), 1, EHAbilityInputID::PrimaryAbility);
		AbilitySystemComp->GrantAbility(ItemToAdd->ItemData->GetItemSecondaryAbility(), 1, EHAbilityInputID::PrimaryHoldAbility);
	}
	else if (ItemType == EItemType::Consumable)
	{
		AbilitySystemComp->GrantAbility(ItemToAdd->ItemData->GetItemPrimaryAbility(), 1, EHAbilityInputID::SecondaryAbility);
		AbilitySystemComp->GrantAbility(ItemToAdd->ItemData->GetItemSecondaryAbility(), 1, EHAbilityInputID::SecondaryHoldAbility);
	}

	// If there is no equipped item of this type equip it
	if ((ItemType == EItemType::Weapon && !ActiveWeapon) ||
		(ItemType == EItemType::Consumable && !ActiveConsumable))
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

	AbilitySystemComp->RemoveLooseGameplayTag(ItemToRemove->ItemData->GetItemTag());

	// Remove the item from the inventory

	// Remove the item abilities from the ASC
	AbilitySystemComp->RemoveAbilityByClass(ItemToRemove->ItemData->GetItemPrimaryAbility());
	AbilitySystemComp->RemoveAbilityByClass(ItemToRemove->ItemData->GetItemSecondaryAbility());

	// Deattach it from the player
	ItemToRemove->DetachFromPlayer();

	InventoryItems.Remove(ItemToRemove);
}

void UHInventoryComponent::EquipItem(AHPlayerItem* ItemToEquip)
{
	// Checks if the player HasAuthority()
	//if (!ItemToEquip || !GetOwner() || !GetOwner()->HasAuthority() || !ASC) return;
	if (!GetOwner() || !GetOwner()->HasAuthority() || !AbilitySystemComp) return;
	
	// GA_Swap send nullptr indicating the player wants to equip melee
	if (ItemToEquip == nullptr)
	{
		// If ActiveWeapon is null that means that the player is already in melee
		if (ActiveWeapon)
		{
			//Unequip the current weapon
			AbilitySystemComp->RemoveLooseGameplayTag(ActiveWeapon->ItemData->GetItemTag());

			UnequipItem(ActiveWeapon);

			SetActiveWeapon(nullptr);
		}
		return;
	}

	// If there is another item of the same type unequip it -> UnequipItem ()
	// Set the new item as active
	//Bind Input from ASC
	// AttachToActiveSocket
	EItemType ItemType = ItemToEquip->ItemData->GetItemType();
	switch (ItemType)
	{
	case EItemType::Weapon:
		{
			if (ActiveWeapon == ItemToEquip)
				return;

			if (ActiveWeapon)
			{
				AbilitySystemComp->RemoveLooseGameplayTag(ActiveWeapon->ItemData->GetItemTag());
				UnequipItem(ActiveWeapon);
			}

			SetActiveWeapon(Cast<AHWeapon>(ItemToEquip));
			AbilitySystemComp->AddLooseGameplayTag(ItemToEquip->ItemData->GetItemTag());
			GetActiveWeapon()->AttachToActiveSocket(Cast<APlayerCharacter>(GetOwner()));
			break;
		}
	case EItemType::Consumable:
		{
			if (ActiveConsumable == ItemToEquip)
				return;

			if (ActiveConsumable)
			{
				AbilitySystemComp->RemoveLooseGameplayTag(ActiveConsumable->ItemData->GetItemTag());
				UnequipItem(ActiveConsumable);
			}

			SetActiveConsumable(Cast<AHConsumable>(ItemToEquip));
			AbilitySystemComp->AddLooseGameplayTag(ItemToEquip->ItemData->GetItemTag());
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
