// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HConsumable.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnConsumableUsed);

class UConsumableDataAsset;

UCLASS()
class HOBOLEAGUEPROJECT_API AHConsumable : public AHPlayerItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHConsumable();

	FOnConsumableUsed OnConsumableUsed;
	
	UFUNCTION()
	void ReduceConsumableCharges();

	UConsumableDataAsset* GetItemConsumableDataAsset(){return ConsumableData;}
	
protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void RestoreConsumableProperties();
	
private:
	UPROPERTY()
	TObjectPtr<UConsumableDataAsset> ConsumableData;
	
	int RemainingCharges;
};
