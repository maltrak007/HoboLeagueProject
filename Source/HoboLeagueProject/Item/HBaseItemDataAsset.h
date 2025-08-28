// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "HBaseItemDataAsset.generated.h"

class UTexture2D;
class USkeletalMesh;
class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;
class UStaticMesh;

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHBaseItemDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	// ======================
	//   Item Effects
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Effects")
	FGameplayTagContainer StatusEffect;

	// ======================
	//   Visuals
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UTexture2D* ItemIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UStaticMesh* ItemMesh;

	// ======================
	//   Abilities
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> ItemPrimaryAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> ItemSecondaryAbility;
	
	// ======== GETTERS ========
	
	UFUNCTION(BlueprintPure, Category = "Item|Effects")
	const FGameplayTagContainer& GetStatusEffect() const { return StatusEffect; }

	UFUNCTION(BlueprintPure, Category = "Item|Visual")
	UTexture2D* GetItemIcon() const { return ItemIcon; }

	UFUNCTION(BlueprintPure, Category = "Item|Visual")
	UStaticMesh* GetItemMesh() const { return ItemMesh; }

	UFUNCTION(BlueprintPure, Category = "Item|Abilities")
	TSubclassOf<UGameplayAbility> GetItemPrimaryAbility() const { return ItemPrimaryAbility; }

	UFUNCTION(BlueprintPure, Category = "Item|Abilities")
	TSubclassOf<UGameplayAbility> GetItemSecondaryAbility() const { return ItemSecondaryAbility; }
};
