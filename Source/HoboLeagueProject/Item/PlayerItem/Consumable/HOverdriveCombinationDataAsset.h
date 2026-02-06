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
	
	const FName& GetCombinationName() const
	{
		return CombinationName;
	}

	int32 GetRequiredNumTags() const
	{
		return NumTagRequired;
	}

	const FGameplayTagContainer& GetRequiredTags() const
	{
		return RequiredTags;
	}

	const TArray<TSubclassOf<UGameplayEffect>>& GetGameplayEffects() const
	{
		return GameplayEffects;
	}

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess="true"))
	FName CombinationName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess="true"))
	int32 NumTagRequired = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess="true"))
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess="true"))
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
	const TArray<FOverdriveDataStruct>& GetDataCombinations() const
	{
		return Combinations;
	}

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Overdrive Combination", meta=(AllowPrivateAccess="true"))
	TArray<FOverdriveDataStruct> Combinations;
};
