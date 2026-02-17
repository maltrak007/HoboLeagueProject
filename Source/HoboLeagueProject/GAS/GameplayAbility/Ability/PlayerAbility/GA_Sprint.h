// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_Sprint.generated.h"

class UCharacterMovementComponent;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_Sprint : public UHBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Sprint();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint|Effects")
	TSubclassOf<UGameplayEffect> SprintGameplayEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprint|Effects")
	float SprintSpeedTarget = 600.f;
private:
	UPROPERTY()
	TObjectPtr<APlayerCharacter> CachedCharacter;
	
	// Active effect handle for stamina cost
	FActiveGameplayEffectHandle SprintEffectHandle;
};
