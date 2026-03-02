// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_UseConsumable.generated.h"

struct FConsumableStatsRow;
class AHConsumable;
class UHAbilityTask_HoldReleaseMinigame;

/**
 * Gameplay Ability for using consumable items
 * Handles animation, minigame, and effect application
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_UseConsumable : public UHBaseGameplayAbility
{
    GENERATED_BODY()
    
public:
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
    
    static FGameplayTag GetUseConsumableEventTag();

protected:
    // ── Configuration ─────────────────────────────────────────────

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* ConsumableMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Minigame")
    TSubclassOf<UUserWidget> HoldReleaseWidgetClass;

    // ── Cached Data ───────────────────────────────────────────────

    const FConsumableStatsRow* CachedConsumableStats = nullptr;
    
    UPROPERTY()
    TObjectPtr<AHConsumable> CachedConsumable = nullptr;

    UPROPERTY()
    TObjectPtr<UHAbilityTask_HoldReleaseMinigame> MinigameTask = nullptr;

    float MinigameScore = 0.0f;

    // ── Lifecycle ─────────────────────────────────────────────────

    UFUNCTION()
    void CacheData();

    UFUNCTION()
    void ClearCacheData();
    
    // ── Event Handlers ────────────────────────────────────────────

    UFUNCTION()
    void OnConsumableEventReceived(FGameplayEventData Data);

    UFUNCTION()
    void OnMinigameComplete(float FinalScore);

    UFUNCTION()
    void OnMinigameCancelled(float FinalScore);

    // ── Minigame ──────────────────────────────────────────────────

    void StartMinigame();

    // ── Effect Application ────────────────────────────────────────

    void ApplyConsumableEffects();
};