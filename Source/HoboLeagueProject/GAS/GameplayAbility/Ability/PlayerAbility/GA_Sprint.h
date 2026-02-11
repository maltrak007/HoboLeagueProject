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
	// Sprint speed multiplier or absolute value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint", meta=(ClampMin="300.0", ClampMax="1000.0"))
	float SprintSpeed = 600.0f;

	// Default walk speed (fallback if we can't read it)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint", meta=(ClampMin="100.0", ClampMax="600.0"))
	float DefaultSpeed = 450.0f;

	// Gameplay Effect for sprint cost (stamina drain)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint|Effects")
	TSubclassOf<UGameplayEffect> SprintCostEffect;

	// Whether to use speed multiplier instead of absolute value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	bool bUseSpeedMultiplier = false;

	// Speed multiplier (if bUseSpeedMultiplier is true)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint",
		meta=(EditCondition="bUseSpeedMultiplier", ClampMin="1.0", ClampMax="3.0"))
	float SpeedMultiplier = 1.5f;

private:
	// Cached references
	UPROPERTY()
	TObjectPtr<APlayerCharacter> CachedCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CachedMoveComp;

	// Active effect handle for stamina cost
	FActiveGameplayEffectHandle SprintEffectHandle;

	// Store original speed to restore it
	float OriginalMaxWalkSpeed = 0.0f;
};
