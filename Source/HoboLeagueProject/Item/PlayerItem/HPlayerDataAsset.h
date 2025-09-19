// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"
#include "HPlayerDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHPlayerDataAsset : public UHBaseItemDataAsset
{
	GENERATED_BODY()
public:
	// ======================
	//   Item Data
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data")
	ERarityType ItemRarity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data")
	EItemSize ItemSize;
	
	// ======================
	//   Visuals
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UTexture2D* ItemIcon;

	// ======================
	//   Abilities
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> ItemPrimaryAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> ItemSecondaryAbility;

	// ======== GETTERS ========
	UFUNCTION(BlueprintPure, Category = "Item|Data")
	const ERarityType& GetItemRarity() const { return ItemRarity; }

	UFUNCTION(BlueprintPure, Category = "Item|Data")
	const EItemSize& GetItemSize() const { return ItemSize; }

	UFUNCTION(BlueprintPure, Category = "Item|Visual")
	UTexture2D* GetItemIcon() const { return ItemIcon; }
	
	UFUNCTION(BlueprintPure, Category = "Item|Abilities")
	TSubclassOf<UGameplayAbility> GetItemPrimaryAbility() const { return ItemPrimaryAbility; }

	UFUNCTION(BlueprintPure, Category = "Item|Abilities")
	TSubclassOf<UGameplayAbility> GetItemSecondaryAbility() const { return ItemSecondaryAbility; }
};
