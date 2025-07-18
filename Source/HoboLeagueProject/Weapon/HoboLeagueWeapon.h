// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class HOBOLEAGUEPROJECT_API AHoboLeagueWeapon : public AActor
{
	GENERATED_BODY()

public:
	AHoboLeagueWeapon();
	
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName;
};
