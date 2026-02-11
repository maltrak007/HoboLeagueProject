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
	if (!IsValid(ItemToAdd) || !GetOwner() || !GetOwner()->HasAuthority() || !AbilitySystemComp) return;
	
	if (InventoryItems.Contains(ItemToAdd)) return;

	const EItemType ItemType = ItemToAdd->ItemData->GetItemType();

	HandleInventoryLimits(ItemType);

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
		if (APlayerCharacter* PlayerOwner = Cast<APlayerCharacter>(GetOwner()))
		{
			ItemToAdd->AttachToHolsterSocket(PlayerOwner);
		}
	}
}

void UHInventoryComponent::HandleInventoryLimits(EItemType ItemType)
{
	// Determine max count based on type
	int32 MaxCount = 0;
	switch (ItemType)
	{
	case EItemType::Weapon:
		MaxCount = MaxWeapons;
		break;
	case EItemType::Consumable:
		MaxCount = MaxConsumables;
		break;
	default:
		return; // No limit for other types
	}

	// Count items of this type
	int32 TypeCount = 0;
	for (const AHPlayerItem* Item : InventoryItems)
	{
		if (IsValid(Item) && Item->ItemData->GetItemType() == ItemType)
		{
			TypeCount++;
		}
	}

	// Remove oldest if limit exceeded
	if (TypeCount >= MaxCount)
	{
		// Iterate forward to find the oldest (first added) item
		for (int32 i = 0; i < InventoryItems.Num(); ++i)
		{
			AHPlayerItem* Existing = InventoryItems[i];
			if (IsValid(Existing) && Existing->ItemData->GetItemType() == ItemType)
			{
				RemoveItem(Existing);
				break;
			}
		}
	}
}

void UHInventoryComponent::RemoveItem(AHPlayerItem* ItemToRemove)
{
	// Checks if the item is valid
	// Checks if the player HasAuthority()
	if (!IsValid(ItemToRemove) || !GetOwner() || !GetOwner()->HasAuthority() || !AbilitySystemComp) return;
	
	const EItemType RemovedType = ItemToRemove->ItemData ? ItemToRemove->ItemData->GetItemType() : EItemType::Other;
	const bool bWasEquipped = IsItemEquipped(ItemToRemove);
	
	if (bWasEquipped)
	{
		UnequipItem(ItemToRemove);
	}
    
	ItemToRemove->DetachFromPlayer();
	InventoryItems.RemoveSwap(ItemToRemove);

	if (bWasEquipped)
	{
		AutoEquipReplacement(RemovedType);
	}
}
bool UHInventoryComponent::IsItemEquipped(const AHPlayerItem* Item) const
{
	if (!IsValid(Item))
	{
		return false;
	}

	// Use type-based checking instead of multiple casts
	const EItemType ItemType = Item->ItemData ? Item->ItemData->GetItemType() : EItemType::Other;
	
	switch (ItemType)
	{
	case EItemType::Weapon:
		return Item == ActiveWeapon;
	case EItemType::Consumable:
		return Item == ActiveConsumable;
	default:
		return false;
	}
}

void UHInventoryComponent::EquipItem(AHPlayerItem* ItemToEquip)
{
	// Checks if the player HasAuthority()
	if (!IsValid(ItemToEquip)||!GetOwner() || !GetOwner()->HasAuthority() || !AbilitySystemComp) return;
	
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

void UHInventoryComponent::AutoEquipReplacement(EItemType ItemType)
{
	// Find first available item of same type
	for (AHPlayerItem* PotentialItem : InventoryItems)
	{
		if (IsValid(PotentialItem) && 
			PotentialItem->ItemData && 
			PotentialItem->ItemData->GetItemType() == ItemType)
		{
			EquipItem(PotentialItem);
			return;
		}
	}
}

void UHInventoryComponent::UnequipItem(AHPlayerItem* ItemToUnequip)
{
	if (!IsValid(ItemToUnequip) || !GetOwner() || !GetOwner()->HasAuthority() || !AbilitySystemComp) return;
	
	AbilitySystemComp->RemoveGrantedAbility(ItemToUnequip->GrantedAbilityHandles);
	
	if (APlayerCharacter* PlayerOwner = Cast<APlayerCharacter>(GetOwner()))
	{
		ItemToUnequip->AttachToHolsterSocket(PlayerOwner);
	}
	
	const EItemType ItemType = ItemToUnequip->ItemData ? ItemToUnequip->ItemData->GetItemType() : EItemType::Other;
	
	switch (ItemType)
	{
	case EItemType::Weapon:
		SetActiveWeapon(nullptr);
		break;
	case EItemType::Consumable:
		SetActiveConsumable(nullptr);
		break;
	default:
		break;
	}
}

void UHInventoryComponent::OnRep_ActiveWeapon()
{
	if (IsValid(ActiveWeapon))
	{
		if (APlayerCharacter* PlayerOwner = Cast<APlayerCharacter>(GetOwner()))
		{
			ActiveWeapon->AttachToActiveSocket(PlayerOwner);
		}
	}
}

void UHInventoryComponent::OnRep_ActiveConsumable()
{
	if (IsValid(ActiveConsumable))
	{
		if (APlayerCharacter* PlayerOwner = Cast<APlayerCharacter>(GetOwner()))
		{
			ActiveConsumable->AttachToActiveSocket(PlayerOwner);
		}
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
