// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HGameplayAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EHAbilityInputID : uint8
{
	None							UMETA(DisplayName = "None"),
	PrimaryAbility					UMETA(DisplayName = "Primary Ability"),
	SecondaryAbility				UMETA(DisplayName = "Secondary Ability"),
	MeleeSlot						UMETA(DisplayName = "Melee Slot"),
	WeaponSlot						UMETA(DisplayName = "Weapon Slot"),
	ConsumableSlot					UMETA(DisplayName = "Consumable Slot"),
	AbilityFive						UMETA(DisplayName = "Ability Five"),
	AbilitySix						UMETA(DisplayName = "Ability Six"),
	Confirm							UMETA(DisplayName = "Confirm"),
	Cancel							UMETA(DisplayName = "Cancel")
	
};

