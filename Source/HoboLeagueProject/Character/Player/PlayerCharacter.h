// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/Character/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UHOverdriveComponent;
class UHInteractionComponent;
class UHStatusHandlerComponent;
class UHInventoryComponent;
class AHBaseItem;
class UInputComponent;

UCLASS()
class HOBOLEAGUEPROJECT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
	
	UHInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	
	UHStatusHandlerComponent* GetStatusHandlerComponent() const { return StatusHandlerComponent; }
	
	UHInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

	UHOverdriveComponent* GetOverdriveComponent() const {return OverdriveComponent; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	void InitAbilityActorInfo();
	
	void InitActorHUD();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHStatusHandlerComponent> StatusHandlerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHOverdriveComponent> OverdriveComponent;
};
