// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HGameplayAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EHAbilityInputID : uint8
{
	None							UMETA(DisplayName = "None"),
	BasicAttack						UMETA(DisplayName = "Basic Attack"),
	SecondaryAttack					UMETA(DisplayName = "Ability One"),
	AbilityTwo						UMETA(DisplayName = "Ability Two"),
	AbilityThree					UMETA(DisplayName = "Ability Three"),
	AbilityFour						UMETA(DisplayName = "Ability Four"),
	AbilityFive						UMETA(DisplayName = "Ability Five"),
	AbilitySix						UMETA(DisplayName = "Ability Six"),
	Confirm							UMETA(DisplayName = "Confirm"),
	Cancel							UMETA(DisplayName = "Cancel")
	
};

