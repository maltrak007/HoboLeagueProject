// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "HOverdriveCombinationDataAsset.generated.h"

class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FOverdriveDataStruct
{
	GENERATED_BODY()

	FName GetCombinationName() const
	{
		return CombinationName;
	}
	
	int GetRequiredNumTags() const
	{
		return NumTagRequired;
	}

	FGameplayTagContainer GetRequiredTags() const
	{
		return RequiredTags;
	}

	TArray<TSubclassOf<UGameplayEffect>> GetGameplayEffects() const
	{
		return GameplayEffects;
	}

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess))
	FName CombinationName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess))
	int NumTagRequired = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess))
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess))
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;
};

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHOverdriveCombinationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	TArray<FOverdriveDataStruct> GetDataCombinations() const
	{
		return Combinations;
	}

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess))
	TArray<FOverdriveDataStruct> Combinations;
};
