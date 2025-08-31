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

	if (bAutoEquipItem)
	{
		EquipItem(Type);
	}
}

void UHInventoryComponent::RemoveItem(AHBaseItem* Item)
{
	if (!Item || !Item->ItemData || !ASC) return;

	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	const EItemType Type = Item->ItemData->GetItemType();

	// Removes the player abilities that were granted from the player character 
	if (Item->ItemData->GetItemPrimaryAbility())
		ASC->RemoveAbilityByClass(Item->ItemData->GetItemPrimaryAbility());
	if (Item->ItemData->GetItemSecondaryAbility())
		ASC->RemoveAbilityByClass(Item->ItemData->GetItemSecondaryAbility());

	// Unbinds them from input
	if (Item->ItemData->GetItemPrimaryAbility())
		ASC->UnbindAbilityByInputID(EHAbilityInputID::PrimaryAbility, Item->ItemData->GetItemPrimaryAbility());
	if (Item->ItemData->GetItemSecondaryAbility())
		ASC->UnbindAbilityByInputID(EHAbilityInputID::SecondaryAbility, Item->ItemData->GetItemSecondaryAbility());

	// Remove from local map
	ItemSlots.Remove(Type);

	// Remove from replicated array
	ReplicatedSlots.RemoveAll([Type](const FItemSlotRep& Slot)
	{
		return Slot.ItemType == Type;
	});

	Item->DetachFromPlayer();

	// Refresh the reference
	if (EquippedItem == Item) EquippedItem = nullptr;

	// Auto-equip another item if available
	for (auto& Pair : ItemSlots)
	{
		if (Pair.Value)
		{
			EquipItem(Pair.Key);
			return;
		}
	}

	// Si no quedan items, bind de habilidades básicas 
	// for (auto& Pair : ASC->GetBasicAbilities())
	// {
	// 	if (Pair.Value)
	// 		ASC->BindAbilityToInputID(Pair.Key, Pair.Value);
	// }
}

void UHInventoryComponent::EquipItem(EItemType ItemType)
{
	if (!ASC) return;

	if (ItemType == EItemType::Melee)
	{
		if (EquippedItem)
		{
			if (EquippedItem->ItemData->GetItemPrimaryAbility())
				ASC->UnbindAbilityByInputID(EHAbilityInputID::PrimaryAbility,
				                            EquippedItem->ItemData->GetItemPrimaryAbility());
			if (EquippedItem->ItemData->GetItemSecondaryAbility())
				ASC->UnbindAbilityByInputID(EHAbilityInputID::SecondaryAbility,
				                            EquippedItem->ItemData->GetItemSecondaryAbility());
		}
		EquippedItem = nullptr; // no actual item

		for (auto& Pair : ASC->GetBasicAbilities())
		{
			if (Pair.Value == UGA_Attack::StaticClass()) continue; // Avoid binding the attack ability twice
			ASC->BindAbilityToInputID(Pair.Key, Pair.Value);
		}

		return;
	}

	AHBaseItem* ItemToEquip = GetItemByType(ItemType);
	if (!ItemToEquip) return;

	EquippedItem = ItemToEquip;

	HandleEquipBindings(EquippedItem);
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

void UHInventoryComponent::HandleEquipBindings(AHBaseItem* ItemToEquip)
{
	// Remove old bindings if there were any
	ASC->UnbindAbilityByInputID(EHAbilityInputID::PrimaryAbility,
	                            ASC->GetBasicAbilities().FindRef(EHAbilityInputID::PrimaryAbility));
	ASC->UnbindAbilityByInputID(EHAbilityInputID::SecondaryAbility,
	                            ASC->GetBasicAbilities().FindRef(EHAbilityInputID::SecondaryAbility));

	// Add the new bindings
	if (ItemToEquip->ItemData->GetItemPrimaryAbility())
		ASC->BindAbilityToInputID(EHAbilityInputID::PrimaryAbility, ItemToEquip->ItemData->GetItemPrimaryAbility());
	if (ItemToEquip->ItemData->GetItemSecondaryAbility())
		ASC->BindAbilityToInputID(EHAbilityInputID::SecondaryAbility, ItemToEquip->ItemData->GetItemSecondaryAbility());
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
