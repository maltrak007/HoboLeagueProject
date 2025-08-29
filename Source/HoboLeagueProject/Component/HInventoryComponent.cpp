// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryComponent.h"

#include "HoboLeagueProject/Character/Player/BasePlayerCharacterState.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/Item/HBaseItem.h"
#include "HoboLeagueProject/Item/HItemType.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"

UHInventoryComponent::UHInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		if (PawnOwner->GetPlayerState())
		{
			ASC = PawnOwner->GetPlayerState()->FindComponentByClass<UHAbilitySystemComponent>();
		}
	}
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

	const EItemType Type = Item->ItemData->GetItemType();

	if (AHBaseItem** ExistingItemPtr = ItemSlots.Find(Type))
	{
		if (*ExistingItemPtr)
		{
			RemoveItem(*ExistingItemPtr);
		}
	}

	ItemSlots.Add(Type, Item);

	// Grant abilities to player
	if (Item->ItemData->GetItemPrimaryAbility())
		ASC->GrantAbility(Item->ItemData->GetItemPrimaryAbility());
	if (Item->ItemData->GetItemSecondaryAbility())
		ASC->GrantAbility(Item->ItemData->GetItemSecondaryAbility());

	EquipItem(Type);
}

void UHInventoryComponent::RemoveItem(AHBaseItem* Item)
{
	if (!Item || !Item->ItemData || !ASC) return;

	const EItemType Type = Item->ItemData->GetItemType();

	//Removes the player abilities that were granted from the player character 
	if (Item->ItemData->GetItemPrimaryAbility())
		ASC->RemoveAbilityByClass(Item->ItemData->GetItemPrimaryAbility());
	if (Item->ItemData->GetItemSecondaryAbility())
		ASC->RemoveAbilityByClass(Item->ItemData->GetItemSecondaryAbility());

	//Unbinds them from input
	if (Item->ItemData->GetItemPrimaryAbility())
		ASC->UnbindAbilityByInputID(EHAbilityInputID::BasicAttack, Item->ItemData->GetItemPrimaryAbility());
	if (Item->ItemData->GetItemSecondaryAbility())
		ASC->UnbindAbilityByInputID(EHAbilityInputID::SecondaryAttack, Item->ItemData->GetItemSecondaryAbility());

	
	ItemSlots.Remove(Type);

	// Detach actor from player
	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Item->SetOwner(nullptr);
	Item->SetActorEnableCollision(true);

	// Si estaba equipado, limpia la referencia
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
	for (auto& Pair : ASC->GetBasicAbilities())
	{
		if (Pair.Value)
			ASC->BindAbilityToInputID(Pair.Key, Pair.Value);
	}
}

void UHInventoryComponent::EquipItem(EItemType ItemType)
{
	if (!ASC) return;

	AHBaseItem* ItemToEquip = GetItemByType(ItemType);
	if (!ItemToEquip) return;

	EquippedItem = ItemToEquip;

	//Remove old bindings if there were any
	ASC->UnbindAbilityByInputID(EHAbilityInputID::BasicAttack, ASC->GetBasicAbilities().FindRef(EHAbilityInputID::BasicAttack));
	ASC->UnbindAbilityByInputID(EHAbilityInputID::SecondaryAttack, ASC->GetBasicAbilities().FindRef(EHAbilityInputID::SecondaryAttack));
	
	//Add the new bindings
	if (ItemToEquip->ItemData->GetItemPrimaryAbility())
		ASC->BindAbilityToInputID(EHAbilityInputID::BasicAttack, ItemToEquip->ItemData->GetItemPrimaryAbility());
	if (ItemToEquip->ItemData->GetItemSecondaryAbility())
		ASC->BindAbilityToInputID(EHAbilityInputID::SecondaryAttack, ItemToEquip->ItemData->GetItemSecondaryAbility());
}

AHBaseItem* UHInventoryComponent::GetItemByType(EItemType ItemType) const
{
	return ItemSlots.FindRef(ItemType);
}
