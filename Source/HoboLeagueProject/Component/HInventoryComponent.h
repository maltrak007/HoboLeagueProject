// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HInventoryComponent.generated.h"


class UHAbilitySystemComponent;
class APlayerCharacter;
enum class EItemType : uint8;
class AHBaseItem;

USTRUCT(BlueprintType)
struct FItemSlotRep
{
	GENERATED_BODY()

	UPROPERTY()
	EItemType ItemType;

	UPROPERTY()
	AHBaseItem* Item;
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
	
	/** (replaces old one if same type exists) */
	void AddItem(AHBaseItem* Item);

	/** Removes item from inventory but does not destroy it, also it will remove the abilities from the player **/
	void RemoveItem(AHBaseItem* Item);

	/** I will use this method to set the current equipped item and grant abilities to the player **/
	void EquipItem(EItemType ItemType);
	
	/** I will use this method to check if it's nullptr to switch to bare hands in the animation system **/
	AHBaseItem* GetEquippedItem() const { return EquippedItem; }

	/** I can use this method to compare the object type when im tracing to put in the HUD 'Pick up' or 'Swap ItemType' if there is an existing one **/
	AHBaseItem* GetItemByType(EItemType ItemType) const;
	
	void InitASC();

protected:
	void HandleEquipBindings(AHBaseItem* ItemToEquip);
	
	UFUNCTION()
	void OnRep_ItemSlots();
	
	UFUNCTION()
	void OnRep_EquippedItem();
	
	UPROPERTY()
	UHAbilitySystemComponent* ASC = nullptr;
	
	/** Stored items */
	
	//Local representation of the inventory, not replicated
	UPROPERTY()
	TMap<EItemType, AHBaseItem*> ItemSlots;
	
	//Replicated representation of the inventory
	//Necessary because TMap is not natively supported for replication
	//Study the FFastArraySerializer for better performance with large arrays
	UPROPERTY(ReplicatedUsing=OnRep_ItemSlots)
	TArray<FItemSlotRep> ReplicatedSlots;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquippedItem)
	AHBaseItem* EquippedItem = nullptr;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	
};
