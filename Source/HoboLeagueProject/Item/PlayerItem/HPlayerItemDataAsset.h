// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"
#include "HPlayerItemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHPlayerItemDataAsset : public UHBaseItemDataAsset
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data")
	FGameplayTag ItemTag;
	// ======================
	//   Visuals
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UTexture2D* ItemIcon;
	
	// ======================
	//   Abilities
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> ItemAbilities;
	
	// ======== GETTERS ========
	UFUNCTION(BlueprintPure, Category = "Item|Data")
	const ERarityType& GetItemRarity() const { return ItemRarity; }

	UFUNCTION(BlueprintPure, Category = "Item|Data")
	const FGameplayTag& GetItemTag() const { return ItemTag; }

	UFUNCTION(BlueprintPure, Category = "Item|Data")
	const EItemSize& GetItemSize() const { return ItemSize; }

	UFUNCTION(BlueprintPure, Category = "Item|Visual")
	UTexture2D* GetItemIcon() const { return ItemIcon; }
	
	UFUNCTION(BlueprintPure, Category = "Item|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GetItemAbilities() const { return ItemAbilities; }
};
