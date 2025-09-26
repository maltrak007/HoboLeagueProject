// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "HoboLeagueProject/GAS/HAttributeSet.h"
#include "GE_StaminaRegeneration.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGE_StaminaRegeneration : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UGE_StaminaRegeneration()
	{
		// Duration: Infinite
		DurationPolicy = EGameplayEffectDurationType::Infinite;

		// Periodic tick every 1 second
		Period = 1.0f;

		// Attribute to modify
		FGameplayModifierInfo ModifierInfo;
		ModifierInfo.Attribute = UHAttributeSet::GetStaminaAttribute(); // <- replace with your attribute
		ModifierInfo.ModifierOp = EGameplayModOp::Additive;
		ModifierInfo.ModifierMagnitude = FScalableFloat(0.5f); // +0.5 per tick

		Modifiers.Add(ModifierInfo);
	}
};
