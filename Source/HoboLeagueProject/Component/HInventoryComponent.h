// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HInventoryComponent.generated.h"


class APlayerCharacter;
class UAbilitySystemComponent;
class UHAbilitySystemComponent;
class AHConsumable;
class AHWeapon;
class AHPlayerItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHInventoryComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(AHPlayerItem* ItemToAdd);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItem(AHPlayerItem* ItemToRemove);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItem(AHPlayerItem* ItemToEquip);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnequipItem(AHPlayerItem* ItemToEquip);

	void LinkAbilitySystemComponent(UAbilitySystemComponent* ASC);
	
	// ** Getters ** //
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<AHPlayerItem*> GetInventoryItems() { return InventoryItems; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<TSubclassOf<AHPlayerItem>>& GetInitialInventoryItems() const { return InitialInventory; } 

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AHWeapon* GetActiveWeapon() const { return ActiveWeapon; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AHConsumable* GetActiveConsumable() const { return ActiveConsumable; }
	
protected:
	UPROPERTY(Transient)
	TObjectPtr<UHAbilitySystemComponent> AbilitySystemComp;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<TSubclassOf<AHPlayerItem>> InitialInventory;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Limits", meta=(ClampMin="1",ClampMax="10"))
	int32 MaxWeapons = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Limits", meta=(ClampMin="1",ClampMax="10"))
	int32 MaxConsumables = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Limits")
	bool bShouldAutoEquip = true;
	
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AHPlayerItem>> InventoryItems;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeapon)
	TObjectPtr<AHWeapon> ActiveWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveConsumable)
	TObjectPtr<AHConsumable> ActiveConsumable;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnRep_ActiveWeapon();

	UFUNCTION()
	void OnRep_ActiveConsumable();
	
	//** Setters **//
	void SetActiveWeapon(AHWeapon* NewActiveWeapon) { ActiveWeapon = NewActiveWeapon; }
	void SetActiveConsumable(AHConsumable* NewActiveConsumable) { ActiveConsumable = NewActiveConsumable; }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
