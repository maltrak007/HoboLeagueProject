// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HoboLeagueProject/GAS/HGameplayAbilityTypes.h"
#include "BaseCharacterController.generated.h"


struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API ABaseCharacterController : public APlayerController
{
	GENERATED_BODY()
public:
	ABaseCharacterController();
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void SetupInputModeForMap();
	/*********************************************************************/
	/*                            INPUT									 */
	/*********************************************************************/
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> DefaultPlayerInputMappingContext;

	void InitInputMapping();
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Hobo | Input")
	TMap<EHAbilityInputID, UInputAction*> GameplayAbilityInputActions;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);

	//void HandleAbilityInput(const FInputActionValue& Value, EHAbilityInputID AbilityInputID);

	void HandleAbilityInputPressed(const FInputActionValue& Value, EHAbilityInputID AbilityInputID);
	void HandleAbilityInputReleased(const FInputActionValue& Value, EHAbilityInputID AbilityInputID);
};
