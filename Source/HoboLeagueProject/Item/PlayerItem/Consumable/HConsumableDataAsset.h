// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Engine/DataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItemDataAsset.h"
#include "HConsumableDataAsset.generated.h"


enum class EGameplayEffectDurationType : uint8;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHConsumableDataAsset : public UHPlayerItemDataAsset
{
	GENERATED_BODY()

public:
	UHConsumableDataAsset();

	// ======================
	//   Consumable Stats
	// ======================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable Stats")
	float Charges;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable Stats")
	// bool bHasManyGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable Stats")
	TSubclassOf<UGameplayEffect> GameplayEffect;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable Stats",
	// 	meta=(EditCondition="bHasManyGameplayEffects", EditConditionHides))
	// TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;


	// ======== GETTERS ========
	UFUNCTION(BlueprintPure, Category = "Consumable|Stats")
	float GetCharges() const { return Charges; }

	// UFUNCTION(BlueprintPure, Category = "Consumable|Stats")
	// bool GetHasManyGameplayEffects() const { return bHasManyGameplayEffects; }
	
	UFUNCTION(BlueprintPure, Category = "Consumable|Stats")
	TSubclassOf<UGameplayEffect> GetGameplayEffect() const { return GameplayEffect; }

	// UFUNCTION(BlueprintPure, Category = "Consumable|Stats")
	// TArray<TSubclassOf<UGameplayEffect>> GetGameplayEffects() const { return GameplayEffects; }
};
