// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_UseConsumable.generated.h"

class UConsumableDataAsset;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_UseConsumable : public UHBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	static FGameplayTag GetUseConsumableEventTag();

	UFUNCTION()
	void ApplyConsumableEffect(FGameplayEventData Data);
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ConsumableMontage;

	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Effect")
	UConsumableDataAsset* ItemConsumableDataAsset;
};
