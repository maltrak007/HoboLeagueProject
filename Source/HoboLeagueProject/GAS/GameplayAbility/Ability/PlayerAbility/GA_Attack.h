// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_Attack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_WaitInputPress;

/**
 * Hybrid Combo Melee Attack System
 * Supports BOTH hold-to-combo (Marvel Rivals) AND press-to-combo (traditional)
 * 
 * Features:
 * - Hold attack button for automatic combo continuation
 * - OR press attack for each combo section (manual)
 * - Switch between styles mid-combo
 * - Per-section damage and stamina costs
 * - Server-authoritative damage
 * - Full multiplayer support with client prediction
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
	
	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	// ============ Configuration ============

	// Attack animation montage with combo sections
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo")
	FName FirstComboSection = "Attack1";
	
	// Default damage effect (fallback if no section-specific effect)
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Damage")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;

	// Damage effect per combo section (e.g., "Attack1" -> GE_MeleeDamage_Light)
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Damage")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap;

	// Default stamina cost (fallback)
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Cost")
	TSubclassOf<UGameplayEffect> DefaultStaminaCost;

	// Stamina cost per combo section
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Cost")
	TMap<FName, TSubclassOf<UGameplayEffect>> StaminaCostMap;

	// Sweep radius for hit detection
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Targeting", meta=(ClampMin="10.0", ClampMax="500.0"))
	float SphereRadiusSweep = 100.0f;

	// Debug visualization for hit detection
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Debug")
	bool bDebugDrawHits = false;

	// ============ Combo Behavior Configuration ============

	// Allow holding attack button to auto-continue combo (Marvel Rivals style)
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo Behavior")
	bool bAllowAutoCombo = true;

	// Allow pressing attack button for each combo section (Traditional style)
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo Behavior")
	bool bAllowManualCombo = true;

	// Combo window duration (how long player has to press/continue combo)
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo Behavior", meta=(ClampMin="0.1", ClampMax="2.0"))
	float ComboWindowDuration = 0.5f;

	// If true, manual press takes priority over auto-combo
	// (useful for players who want precise control)
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo Behavior")
	bool bManualPressTakesPriority = false;

	// ============ Runtime State ============

	// Next combo section to play (set by animation notifies)
	FName NextComboName = NAME_None;

	// Is the attack input currently being held?
	bool bIsInputHeld = false;

	// Has the player manually pressed during the current combo window?
	bool bManualPressReceived = false;

	// Is there an active combo window?
	bool bInComboWindow = false;

	// Timer for combo window expiration
	FTimerHandle ComboWindowTimer;

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

	// ============ Montage Callbacks ============

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageBlendOut();

	// ============ Input Callbacks ============

	UFUNCTION()
	void OnManualComboInput(float TimeWaited);

	// ============ Event Callbacks ============

	UFUNCTION()
	void OnComboChangeEvent(FGameplayEventData Data);

	UFUNCTION()
	void OnDamageEvent(FGameplayEventData Data);

	UFUNCTION()
	void OnStaminaEvent(FGameplayEventData Data);

	// ============ Combo Logic ============

	// Try to commit the queued combo
	void TryCommitCombo();

	// Called when combo window expires
	void OnComboWindowExpired();

	// Check if we can afford stamina for a section
	bool CanPayStaminaForSection(FName SectionName) const;

	// Get damage effect for current combo section
	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const;

	// Get stamina cost for current combo section
	TSubclassOf<UGameplayEffect> GetStaminaCostForCurrentCombo() const;

	// Apply stamina cost
	void ConsumeStamina();

	// Deal damage to hit targets
	void DealDamage(const FGameplayAbilityTargetDataHandle& TargetData);

	// ============ Gameplay Tag Helpers ============

	FGameplayTag GetComboChangeEventTag() const;
	FGameplayTag GetComboEndEventTag() const;
	FGameplayTag GetDamageEventTag() const;
	FGameplayTag GetStaminaEventTag() const;
	bool ShouldChainAttack() const;
};
