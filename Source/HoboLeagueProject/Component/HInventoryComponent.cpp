// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryComponent.h"

#include "Components/SphereComponent.h"
#include "HoboLeagueProject/Character/Player/BasePlayerCharacterState.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/GameplayAbilities/Abilities/GA_Attack.h"
#include "HoboLeagueProject/Item/HBaseItem.h"
#include "HoboLeagueProject/Item/HItemType.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"
#include "Net/UnrealNetwork.h"

UHInventoryComponent::UHInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	//Subscribe to the delegate that will be called when the player is dead
}

void UHInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASC && EndPlayReason == EEndPlayReason::Destroyed)
	{
		ASC->RemoveAllGrantedAbilities();
	}

	ItemSlots.Empty();
	EquippedItem = nullptr;

	Super::EndPlay(EndPlayReason);
}

void UHInventoryComponent::AddItem(AHBaseItem* Item)
{
	if (!Item || !Item->ItemData || !ASC) return;

	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	const EItemType Type = Item->ItemData->GetItemType();

	//If exists remove it from the inventory
	if (AHBaseItem** ExistingItemPtr = ItemSlots.Find(Type))
	{
		if (*ExistingItemPtr)
		{
			RemoveItem(*ExistingItemPtr);
		}
	}

	// Update local map
	ItemSlots.Add(Type, Item);

	// Update replication array
	FItemSlotRep NewSlot;
	NewSlot.ItemType = Type;
	NewSlot.Item = Item;
	ReplicatedSlots.Add(NewSlot);

	// Grant abilities to player
	if (Item->ItemData->GetItemPrimaryAbility())
		ASC->GrantAbility(Item->ItemData->GetItemPrimaryAbility());
	if (Item->ItemData->GetItemSecondaryAbility())
		ASC->GrantAbility(Item->ItemData->GetItemSecondaryAbility());

	//** ACTIVATE IT IN PLAYER_SETTINGS **//
	if (bAutoEquipItem)
	{
		EquipItem(Type);
	}
}

void UHInventoryComponent::RemoveItem(AHBaseItem* Item)
{
	if (!Item || !Item->ItemData || !ASC) return;

	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	//* If the item being removed is the currently equipped one, we need to unbind its abilities *//
	if (Item == EquippedItem)
	{
		// Unbinds them from input
		if (Item->ItemData->GetItemPrimaryAbility())
			ASC->UnbindAbilityByInputID_Class(EHAbilityInputID::PrimaryAbility,
			                                  Item->ItemData->GetItemPrimaryAbility());
		if (Item->ItemData->GetItemSecondaryAbility())
			ASC->UnbindAbilityByInputID_Class(EHAbilityInputID::SecondaryAbility,
			                                  Item->ItemData->GetItemSecondaryAbility());

		//Update the reference
		EquippedItem = nullptr;

		// Auto-equip another item if available
		for (auto& Pair : ItemSlots)
		{
			if (Pair.Value)
			{
				EquipItem(Pair.Key);
				break;
			}
		}
	}

	// Removes the player abilities that were granted from the player character 
	if (Item->ItemData->GetItemPrimaryAbility())
		ASC->RemoveAbilityByClass(Item->ItemData->GetItemPrimaryAbility());
	if (Item->ItemData->GetItemSecondaryAbility())
		ASC->RemoveAbilityByClass(Item->ItemData->GetItemSecondaryAbility());

	const EItemType Type = Item->ItemData->GetItemType();

	// Remove from local map
	ItemSlots.Remove(Type);

	// Remove from replicated array
	ReplicatedSlots.RemoveAll([Type](const FItemSlotRep& Slot)
	{
		return Slot.ItemType == Type;
	});

	// If no items are left, bind the basic abilities (Melee)
	if (ItemSlots.IsEmpty())
	{
		for (auto& Pair : ASC->GetBasicAbilities())
		{
			if (Pair.Value)
				ASC->BindAbilityToInputID(Pair.Key, Pair.Value);
		}
	}

	Item->DetachFromPlayer();
}

void UHInventoryComponent::EquipItem(EItemType ItemType)
{
	if (!ASC) return;

	AHBaseItem* ItemToEquip = GetItemByType(ItemType);

	if (!ItemToEquip)
	{
		ASC->UnbindAllAbilitiesFromInputID(EHAbilityInputID::PrimaryAbility);
		ASC->UnbindAllAbilitiesFromInputID(EHAbilityInputID::SecondaryAbility);
		for (auto& Pair : ASC->GetBasicAbilities())
		{
			if (Pair.Value)
				ASC->BindAbilityToInputID(Pair.Key, Pair.Value);
		}
		EquippedItem->AttachToHolsterSocket(Cast<APlayerCharacter>(GetOwner()));
		EquippedItem = nullptr;
		return;
	}

	if (EquippedItem != nullptr)
	{
		EquippedItem->AttachToHolsterSocket(Cast<APlayerCharacter>(GetOwner()));
	}

	HandleEquipBindings(ItemToEquip);
}

void UHInventoryComponent::HandleEquipBindings(AHBaseItem* ItemToEquip)
{
	if (!ASC || !ItemToEquip) return;

	// Already equipped? Do nothing
	if (EquippedItem == ItemToEquip)
	{
		return;
	}

	// Unbind previous equipped item’s abilities if any
	ASC->UnbindAllAbilitiesFromInputID(EHAbilityInputID::PrimaryAbility);
	ASC->UnbindAllAbilitiesFromInputID(EHAbilityInputID::SecondaryAbility);
	
	EquippedItem = ItemToEquip;

	// Bind the new abilities
	if (ItemToEquip->ItemData->GetItemPrimaryAbility())
		ASC->BindAbilityToInputID(EHAbilityInputID::PrimaryAbility, ItemToEquip->ItemData->GetItemPrimaryAbility());
	if (ItemToEquip->ItemData->GetItemSecondaryAbility())
		ASC->BindAbilityToInputID(EHAbilityInputID::SecondaryAbility, ItemToEquip->ItemData->GetItemSecondaryAbility());

	EquippedItem->AttachToActiveSocket(Cast<APlayerCharacter>(GetOwner()));
}

void UHInventoryComponent::OnRep_ItemSlots()
{
	// Aquí el cliente puede refrescar UI (inventario visual).
	ItemSlots.Empty();
	for (const FItemSlotRep& Slot : ReplicatedSlots)
	{
		ItemSlots.Add(Slot.ItemType, Slot.Item);
	}
}

AHBaseItem* UHInventoryComponent::GetItemByType(EItemType ItemType) const
{
	return ItemSlots.FindRef(ItemType);
}

void UHInventoryComponent::LinkAbilitySystemComponent()
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		if (APlayerState* PS = PawnOwner->GetPlayerState())
		{
			ASC = PS->FindComponentByClass<UHAbilitySystemComponent>();
		}
	}
}

void UHInventoryComponent::OnRep_EquippedItem()
{
	// Client actualize and bind the abilities
	HandleEquipBindings(EquippedItem);
}

void UHInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHInventoryComponent, ReplicatedSlots);
	DOREPLIFETIME(UHInventoryComponent, EquippedItem);
}
