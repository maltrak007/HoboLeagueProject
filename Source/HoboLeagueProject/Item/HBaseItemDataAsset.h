// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "HBaseItemDataAsset.generated.h"

enum class EItemSize : uint8;
enum class ERarityType : uint8;
enum class EItemType : uint8;
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
	// ======== GETTERS ========
	UFUNCTION(BlueprintPure, Category = "Item|Data")
	const EItemType& GetItemType() const { return ItemType; }
	
	UFUNCTION(BlueprintPure, Category = "Item|Visual")
	UStaticMesh* GetItemMesh() const { return ItemMesh; }

	UFUNCTION(BlueprintPure, Category = "Item|Visual")
	USkeletalMesh* GetItemSkeletalMesh() const { return ItemSkeletalMesh; }
	
protected:
	// ======================
	//   Item Data
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data")
	EItemType ItemType;
	// ======================
	//   Visuals
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UStaticMesh* ItemMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	USkeletalMesh* ItemSkeletalMesh;
};
