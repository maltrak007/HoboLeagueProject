// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_RegenerateStamina.generated.h"

/**
 * 
 */
class UGameplayEffect;

UCLASS()
class HOBOLEAGUEPROJECT_API UGA_RegenerateStamina : public UHBaseGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_RegenerateStamina();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StaminaRegenerationEffect;
	
	FActiveGameplayEffectHandle ActiveStaminaDrain;
};
