// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnWeaponHit);

class USphereComponent;
class UHWeaponDataAsset;
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
	
	UFUNCTION()
	void RestoreWeaponProperties();
	
private:
	UPROPERTY()
	TObjectPtr<UHWeaponDataAsset> WeaponData;
	
	float RemainingDurability;
};
