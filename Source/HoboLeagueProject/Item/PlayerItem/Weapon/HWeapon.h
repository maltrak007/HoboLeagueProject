// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HWeapon.generated.h"

class USphereComponent;
class UWeaponDataAsset;
class APlayerCharacter;

UCLASS()
class HOBOLEAGUEPROJECT_API AHWeapon : public AHPlayerItem
{
	GENERATED_BODY()

public:
	AHWeapon();
	
protected:
	virtual void BeginPlay() override;

	
private:

};
