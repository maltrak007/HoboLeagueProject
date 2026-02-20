// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Engine/DataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItemDataAsset.h"
#include "HConsumableDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHConsumableDataAsset : public UHPlayerItemDataAsset
{
	GENERATED_BODY()

public:
	UHConsumableDataAsset();
	
	UFUNCTION(BlueprintPure, Category = "Consumable|Stats")
	TSubclassOf<UGameplayEffect> GetInstantGameplayEffect() const { return InstantGameplayEffect; }

	UFUNCTION(BlueprintPure, Category = "Consumable|Stats")
	TSubclassOf<UGameplayEffect> GetDurationGameplayEffect() const { return DurationGameplayEffect; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable Stats")
	TSubclassOf<UGameplayEffect> InstantGameplayEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable Stats")
	TSubclassOf<UGameplayEffect> DurationGameplayEffect;
};
