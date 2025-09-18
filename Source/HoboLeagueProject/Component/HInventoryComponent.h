// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HInventoryComponent.generated.h"

class AHPlayerItem;
class UHAbilitySystemComponent;
enum class EItemType : uint8;

USTRUCT(BlueprintType)
struct FItemSlotRep
{
	GENERATED_BODY()

	UPROPERTY()
	EItemType ItemType;

	UPROPERTY()
	AHPlayerItem* Item;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHInventoryComponent();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Add the abilities that the item holds and adds it to the TMap */
	void AddItem(AHPlayerItem* Item);

	/** Removes item from inventory but does not destroy it, also it will remove the abilities and binds from the player **/
	void RemoveItem(AHPlayerItem* Item);

	/** I will use this method to set the current equipped item and grant abilities to the player **/
	void EquipItem(EItemType ItemType);

	/** Iterates inside the TMap looking for the next position available and if its possible put the actual equippedItem as selected **/
	//void QuickSwapItem(EItemType Item);
	
	/** I will use this method to check if it's nullptr to switch to bare hands in the animation system **/
	UFUNCTION(BlueprintCallable)
	AHPlayerItem* GetEquippedItem() const { return EquippedItem; }

	/** I can use this method to compare the object type when im tracing to put in the HUD 'Pick up' or 'Swap ItemType' if there is an existing one **/
	AHPlayerItem* GetItemByType(EItemType ItemType) const;

	/** Link the Ability System Component from the player state **/
	void LinkAbilitySystemComponent();

protected:
	UPROPERTY()
	UHAbilitySystemComponent* ASC = nullptr;
	
	//Local representation of the inventory, not replicated
	UPROPERTY()
	TMap<EItemType, AHPlayerItem*> ItemSlots;

	//Replicated representation of the inventory
	//Necessary because TMap is not natively supported for replication
	//Study the FFastArraySerializer for better performance with large arrays
	UPROPERTY(ReplicatedUsing=OnRep_ItemSlots)
	TArray<FItemSlotRep> ReplicatedSlots;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedItem)
	AHPlayerItem* EquippedItem = nullptr;

	/** Setting if the item is auto-equipped when picked up **/
	UPROPERTY(EditDefaultsOnly)
	bool bAutoEquipItem = false;

	UFUNCTION()
	void HandlePlayerDeath();
	
	void HandleEquipBindings(AHPlayerItem* ItemToEquip);
	
	UFUNCTION()
	void OnRep_ItemSlots();
	
	UFUNCTION()
	void OnRep_EquippedItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
