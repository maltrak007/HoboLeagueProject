// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HGameplayAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EHAbilityInputID : uint8
{
	None							UMETA(DisplayName = "None"),
	PrimaryAbility					UMETA(DisplayName = "Primary Ability"),
	PrimaryHoldAbility				UMETA(DisplayName = "Primary Hold Ability"),
	SecondaryAbility				UMETA(DisplayName = "Secondary Ability"),
	SecondaryHoldAbility			UMETA(DisplayName = "Secondary Hold Ability"),
	AimAbility						UMETA(DisplayName = "Aim Ability"),
	WeaponSlot						UMETA(DisplayName = "Weapon Slot"),
	ConsumableSlot					UMETA(DisplayName = "Consumable Slot"),
	Interact						UMETA(DisplayName = "Interact"),
	AbilitySix						UMETA(DisplayName = "Ability Six"),
	Confirm							UMETA(DisplayName = "Confirm"),
	Cancel							UMETA(DisplayName = "Cancel")
	
};

