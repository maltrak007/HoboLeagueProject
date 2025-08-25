// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/Character/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UInputComponent;
class AHWeapon;

UCLASS()
class HOBOLEAGUEPROJECT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
	
	void EquipWeapon(AHWeapon* Weapon);

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AHWeapon* Weapon);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	void InitAbilityActorInfo();
	
};
