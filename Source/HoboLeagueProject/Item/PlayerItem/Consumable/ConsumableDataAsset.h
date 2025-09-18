// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"
#include "ConsumableDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UConsumableDataAsset : public UHBaseItemDataAsset
{
	GENERATED_BODY()
public:
	UConsumableDataAsset();

	// ======================
	//   Consumable Stats
	// ======================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable Stats")
	int Charges;
	
	// ======== GETTERS ========
	UFUNCTION(BlueprintPure, Category = "Consumable|Stats")
	int GetCharges() const { return Charges; }

};
