// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_Attack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_WaitInputPress;

/**
 * Combo-based melee attack ability
 * Features:
 * - Dynamic combo chains via animation notifies
 * - Per-section damage and stamina costs
 * - Input buffering for smooth combos
 * - Server-authoritative damage
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_Attack : public UHBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Attack();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	// ============ Configuration ============
	
	// Attack animation montage
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	// Default damage effect (fallback)
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Damage")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;

	// Damage effect per combo section
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Damage")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap;

	// Default stamina cost (fallback)
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Cost")
	TSubclassOf<UGameplayEffect> DefaultStaminaCost;

	// Stamina cost per combo section
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Cost")
	TMap<FName, TSubclassOf<UGameplayEffect>> StaminaCostMap;

	// Sweep radius for hit detection
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Targeting")
	float SphereRadiusSweep = 100.0f;

	// Enable debug visualization
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Debug")
	bool bDebugDrawHits = false;

	// ============ Combo State ============
	
	// Next combo section to play (set by anim notifies)
	FName NextComboName = NAME_None;

	// Whether we're waiting for combo input
	bool bIsWaitingForComboInput = false;

	// ============ Task References ============
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputPress> InputTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboChangeTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> DamageTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> StaminaTask;

	// ============ Setup Functions ============
	
	void SetupMontageTask();
	void SetupEventTasks();
	void SetupInputTask();

	// ============ Callbacks ============
	
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageBlendOut();

	UFUNCTION()
	void OnComboInputPressed(float TimeWaited);

	UFUNCTION()
	void OnComboChangeEvent(FGameplayEventData Data);

	UFUNCTION()
	void OnDamageEvent(FGameplayEventData Data);

	UFUNCTION()
	void OnStaminaEvent(FGameplayEventData Data);

	// ============ Combo Logic ============
	
	// Try to advance to next combo section
	void TryCommitCombo();

	// Check if we can afford the stamina cost for a section
	bool CanPayStaminaForSection(FName SectionName) const;

	// Get appropriate damage effect for current combo section
	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const;

	// Get appropriate stamina cost for current combo section
	TSubclassOf<UGameplayEffect> GetStaminaCostForCurrentCombo() const;

	// Apply stamina cost for current section
	void ConsumeStamina();

	// Deal damage to hit targets
	void DealDamage(const FGameplayAbilityTargetDataHandle& TargetData);

	// ============ Gameplay Tag Helpers ============
	
	FGameplayTag GetComboChangeEventTag() const;
	FGameplayTag GetComboEndEventTag() const;
	FGameplayTag GetDamageEventTag() const;
	FGameplayTag GetStaminaEventTag() const;
};