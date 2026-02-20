#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_Attack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_WaitInputPress;
class AHWeapon;
struct FWeaponStatsRow;

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
    // ── Configuration ─────────────────────────────────────────────

    UPROPERTY(EditDefaultsOnly, Category = "Attack|Animation")
    TObjectPtr<UAnimMontage> AttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Attack|Animation")
    FName FirstComboSection = FName("Attack1");

    // ── Combo System ──────────────────────────────────────────────
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Animation")
    TMap<FName, bool> ChargedAttackSections;
    
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo")
    bool bAllowAutoCombo = true;

    UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo")
    bool bAllowManualCombo = true;

    UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo")
    bool bManualPressTakesPriority = false;

    UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo")
    float ComboWindowDuration = 0.5f;

    // ── Effects (now using SetByCaller from weapon stats) ────────

    // Damage effect with SetByCaller for dynamic damage values
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Effects")
    TSubclassOf<UGameplayEffect> DamageEffect;

    // Stamina cost effect with SetByCaller for dynamic costs
    UPROPERTY(EditDefaultsOnly, Category = "Attack|Effects")
    TSubclassOf<UGameplayEffect> StaminaCostEffect;

    // ── Hit Detection ─────────────────────────────────────────────

    UPROPERTY(EditDefaultsOnly, Category = "Attack|Hit Detection")
    float SphereRadiusSweep = 50.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Attack|Hit Detection")
    bool bDebugDrawHits = false;

    UPROPERTY()
    // Track which actors we've hit in THIS damage event
    TArray<TWeakObjectPtr<AActor>> CurrentEventIgnoredActors;

    UPROPERTY()
    TArray<AActor*> CachedIgnoreActors;

    // ── Runtime State ─────────────────────────────────────────────
    FName NextComboName = NAME_None;
    bool bIsInputHeld = false;
    bool bManualPressReceived = false;
    bool bInComboWindow = false;
    bool bShouldChainAttack = false;

    FTimerHandle ComboWindowTimer;
    
    const FWeaponStatsRow* CachedWeaponStats = nullptr;
    
    UPROPERTY()
    TObjectPtr<AHWeapon> CachedWeapon = nullptr;
    
    UPROPERTY()
    TObjectPtr<UAnimInstance> CachedAnimInstance = nullptr;
    
    FGameplayTag Tag_ComboChange;
    FGameplayTag Tag_ComboEnd;
    FGameplayTag Tag_Damage;
    FGameplayTag Tag_Stamina;
    FGameplayTag Tag_StaminaCost;
    
    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboChangeTask;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitGameplayEvent> DamageTask;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitGameplayEvent> StaminaTask;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitInputPress> InputTask;

    // ── Setup ─────────────────────────────────────────────────────

    void SetupMontageTask();
    void SetupEventTasks();
    void SetupInputTask();

    // ── Montage Callbacks ─────────────────────────────────────────

    UFUNCTION()
    void OnMontageCompleted();

    UFUNCTION()
    void OnMontageCancelled();

    UFUNCTION()
    void OnMontageInterrupted();

    UFUNCTION()
    void OnMontageBlendOut();

    // ── Input Callbacks ───────────────────────────────────────────

    UFUNCTION()
    void OnManualComboInput(float TimeWaited);

    // ── Event Callbacks ───────────────────────────────────────────

    UFUNCTION()
    void OnComboChangeEvent(FGameplayEventData Data);

    UFUNCTION()
    void OnDamageEvent(FGameplayEventData Data);

    UFUNCTION()
    void OnStaminaEvent(FGameplayEventData Data);

    // ── Combo Logic ───────────────────────────────────────────────

    void TryCommitCombo();
    void OnComboWindowExpired();

    // ── Weapon & Stats Access ─────────────────────────────────────

    AHWeapon* GetActiveWeapon() const;
    const FWeaponStatsRow* GetWeaponStats() const;

    // ── Stamina ───────────────────────────────────────────────────

    bool CanPayStaminaForAttack() const;
    void ConsumeStamina();
    float GetStaminaCostForCurrentSection() const;

    // ── Damage ────────────────────────────────────────────────────

    void DealDamage(const FGameplayAbilityTargetDataHandle& TargetData);

    // ── Helpers ───────────────────────────────────────────────────
    void CacheAbilityData();
    void ClearCachedData();
    FGameplayTag GetComboChangeEventTag() const;
    FGameplayTag GetComboEndEventTag() const;
    FGameplayTag GetDamageEventTag() const;
    FGameplayTag GetStaminaEventTag() const;
    FGameplayTag GetStaminaCostEventTag() const;

    bool ShouldChainAttack() const;
};
