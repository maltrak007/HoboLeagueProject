// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryComponent.h"

#include "HoboLeagueProject/Character/Player/BasePlayerCharacterState.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/Item/HItemType.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerDataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "Net/UnrealNetwork.h"

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
	if (ASC && EndPlayReason == EEndPlayReason::Destroyed)
	{
		ASC->RemoveAllGrantedAbilities();
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Status.Dead")),
		                              EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}

	ItemSlots.Empty();
	EquippedItem = nullptr;

	Super::EndPlay(EndPlayReason);
}

void UHInventoryComponent::LinkAbilitySystemComponent()
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		if (APlayerState* PS = PawnOwner->GetPlayerState())
		{
			ASC = PS->FindComponentByClass<UHAbilitySystemComponent>();

			if (ASC)
			{
				ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Status.Dead")),
				                              EGameplayTagEventType::NewOrRemoved).AddUObject(
					this, &UHInventoryComponent::OnDeathTagChanged);
			}
		}
	}
}

void UHInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHInventoryComponent, ReplicatedSlots);
	DOREPLIFETIME(UHInventoryComponent, EquippedItem);
}

void UHInventoryComponent::OnDeathTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		HandlePlayerDeath();
	}
}

void UHInventoryComponent::AddItem(AHPlayerItem* Item)
{
	if (!Item || !Item->ItemData || !ASC) return;

	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	EItemType Type = Item->ItemData->GetItemType();

	//If exists remove it from the inventory
	if (AHPlayerItem** ExistingItemPtr = ItemSlots.Find(Type))
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
	else
	{
		Item->AttachToHolsterSocket(Cast<APlayerCharacter>(GetOwner()));
	}
}

void UHInventoryComponent::RemoveItem(AHPlayerItem* Item)
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

	EquippedItem = nullptr;

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

	AHPlayerItem* ItemToEquip = GetItemByType(ItemType);

	// If the item type does not exist in the inventory, equip melee
	if (!ItemToEquip && EquippedItem != nullptr)
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

		//Include here the future FOnWeaponEquipped
		return;
	}

	if (EquippedItem != nullptr && EquippedItem != ItemToEquip)
	{
		EquippedItem->AttachToHolsterSocket(Cast<APlayerCharacter>(GetOwner()));
	}

	HandleEquipBindings(ItemToEquip);
}

void UHInventoryComponent::HandleEquipBindings(AHPlayerItem* ItemToEquip)
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

	//Include here the future FOnWeaponEquipped
}

AHPlayerItem* UHInventoryComponent::GetItemByType(EItemType ItemType) const
{
	return ItemSlots.FindRef(ItemType);
}

void UHInventoryComponent::HandlePlayerDeath()
{
	for (auto& Pair : ItemSlots)
	{
		if (AHPlayerItem* Item = Pair.Value)
		{
			RemoveItem(Item);
		}
	}

	ItemSlots.Empty();
	EquippedItem = nullptr;
}

//TODO:: REPLICATION MIGHT CAUSE THE BIND TO HAPPEN TWICE, CHECK IT
void UHInventoryComponent::OnRep_EquippedItem()
{
	// Client actualize and bind the abilities
	HandleEquipBindings(EquippedItem);
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
