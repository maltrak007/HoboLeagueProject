// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/HBaseGameplayAbility.h"
#include "GA_ProtoAttack.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_ProtoAttack : public UHBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	
protected:
	
private:
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackMontage;
};
