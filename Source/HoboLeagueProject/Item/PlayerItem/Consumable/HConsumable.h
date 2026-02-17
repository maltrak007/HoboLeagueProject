// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HConsumable.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnConsumableUsed);

class UHConsumableDataAsset;

UCLASS()
class HOBOLEAGUEPROJECT_API AHConsumable : public AHPlayerItem
{
	GENERATED_BODY()

public:
	AHConsumable();

	FOnConsumableUsed OnConsumableUsed;
	
	UFUNCTION()
	void ReduceConsumableCharges(float _amountToReduce);

	UHConsumableDataAsset* GetItemConsumableDataAsset(){return ConsumableData;}
	
protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void RestoreConsumableProperties();
	
private:
	UPROPERTY()
	TObjectPtr<UHConsumableDataAsset> ConsumableData;
	
	float RemainingCharges;
};
