// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_UseConsumable.generated.h"

class AHConsumable;
class APlayerCharacter;
class UHConsumableDataAsset;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_UseConsumable : public UHBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_UseConsumable();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	static FGameplayTag GetUseConsumableEventTag();

private:
	UFUNCTION()
	void ApplyConsumableEffect(FGameplayEventData Data);
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ConsumableMontage;
};
