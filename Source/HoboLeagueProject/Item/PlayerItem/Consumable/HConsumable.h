// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HConsumable.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnConsumableUsed);
struct FConsumableStatsRow;
class UHConsumableDataAsset;

UCLASS()
class HOBOLEAGUEPROJECT_API AHConsumable : public AHPlayerItem
{
	GENERATED_BODY()

public:
	AHConsumable();

	FOnConsumableUsed OnConsumableUsed;
	
	UHConsumableDataAsset* GetItemConsumableDataAsset(){return ConsumableData;}
	
	const FConsumableStatsRow* GetConsumableStats() const;

	// Polymorphic override
	virtual const void* GetStatsForRarity() const override
	{
		return GetConsumableStats();
	}
	
	UFUNCTION(BlueprintCallable, Category = "Consumable|Stats")
	bool GetConsumableStatsValue(FConsumableStatsRow& OutStats) const;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TObjectPtr<UHConsumableDataAsset> ConsumableData;
	
};
