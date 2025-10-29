// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HInventoryComponent.generated.h"


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
	TArray<AHPlayerItem*> GetInventoryItems() { return InventoryItems; }

	AHWeapon* GetActiveWeapon() const { return ActiveWeapon; }
	
	AHConsumable* GetActiveConsumable() const { return ActiveConsumable; }
	
protected:
	TObjectPtr<UHAbilitySystemComponent> AbilitySystemComp;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Limits")
	int numMaxWeapons = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Limits")
	int numMaxConsumables = 2;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AHPlayerItem>> InventoryItems;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeapon)
	AHWeapon* ActiveWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveConsumable)
	AHConsumable* ActiveConsumable;

	UFUNCTION()
	void OnRep_ActiveWeapon();

	UFUNCTION()
	void OnRep_ActiveConsumable();
	
	//** Setters **//
	void SetActiveWeapon(AHWeapon* NewActiveWeapon) { ActiveWeapon = NewActiveWeapon; }
	void SetActiveConsumable(AHConsumable* NewActiveConsumable) { ActiveConsumable = NewActiveConsumable; }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
