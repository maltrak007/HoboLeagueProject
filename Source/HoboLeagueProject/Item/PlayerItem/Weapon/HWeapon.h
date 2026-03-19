// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HWeapon.generated.h"

struct FWeaponStatsRow;
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
	
	const FWeaponStatsRow* GetWeaponStats() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UHWeaponDataAsset> WeaponData;
};
