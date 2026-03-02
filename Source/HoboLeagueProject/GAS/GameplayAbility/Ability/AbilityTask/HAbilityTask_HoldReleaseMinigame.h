// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumableStatTable.h"
#include "HAbilityTask_HoldReleaseMinigame.generated.h"

class UUserWidget;
class UHHoldReleaseMinigame;

/**
 * Hold-Release Minigame Task
 * Manages the minigame logic and lifetime
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHAbilityTask_HoldReleaseMinigame : public UAbilityTask
{
    GENERATED_BODY()

public:
    // ── Delegates ─────────────────────────────────────────────────

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMinigameCompleteSignature, float, FinalScore);
    
    UPROPERTY(BlueprintAssignable)
    FMinigameCompleteSignature OnCompleted;

    UPROPERTY(BlueprintAssignable)
    FMinigameCompleteSignature OnCancelled;

    // ── Factory ───────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", 
        meta=(DisplayName="Play Hold-Release Minigame", HidePin="OwningAbility", DefaultToSelf="OwningAbility"))
    static UHAbilityTask_HoldReleaseMinigame* PlayHoldReleaseMinigame(
        UGameplayAbility* OwningAbility,
        TSubclassOf<UUserWidget> WidgetClass,
        const FConsumableStatsRow& ConsumableStats,
        float StartPosition
    );

    // ── Lifecycle ─────────────────────────────────────────────────

    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;
    virtual void TickTask(float DeltaTime) override;

    // ── Input ─────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Minigame")
    void OnInputReleased();

    UFUNCTION(BlueprintCallable, Category = "Minigame")
    void CancelMinigame();

protected:
    // ── Configuration ─────────────────────────────────────────────

    UPROPERTY()
    TSubclassOf<UUserWidget> MinigameWidgetClass;

    FConsumableStatsRow ConsumableConfig;

    float StartPos = 100.0f;
    float EndPos = 0.0f;

    const float SlideDuration = 1.5f;
    const float SlideDistance = 20.0f;

    // ── Runtime State ─────────────────────────────────────────────

    UPROPERTY()
    TObjectPtr<UHHoldReleaseMinigame> MinigameWidget;

    bool bIsHolding = false;
    bool bIsSliding = false;
    float CurrentPosition = 0.0f;
    float SlideStartPosition = 0.0f;
    float SlideElapsedTime = 0.0f;

    // ── Internal Logic ────────────────────────────────────────────

    void UpdateIndicator(float DeltaTime);
    void StartSlide();
    void UpdateSlide(float DeltaTime);
    float CalculateFinalScore() const;
    void CompleteMinigame();
};