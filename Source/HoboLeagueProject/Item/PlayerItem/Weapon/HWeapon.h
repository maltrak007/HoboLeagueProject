// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnWeaponHit);

class USphereComponent;
class UWeaponDataAsset;
class APlayerCharacter;

UCLASS()
class HOBOLEAGUEPROJECT_API AHWeapon : public AHPlayerItem
{
	GENERATED_BODY()

public:
	AHWeapon();

	FOnWeaponHit OnWeaponHit;
	
	UFUNCTION()
	void ReduceWeaponDurability();
	
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void RestoreWeaponProperties();
	
private:
	// Need to create a pointer to the custom UDataAsset to access the custom properties that the weapon has //
	UPROPERTY()
	TObjectPtr<UWeaponDataAsset> WeaponData;
	
	float RemainingDurability;
};
