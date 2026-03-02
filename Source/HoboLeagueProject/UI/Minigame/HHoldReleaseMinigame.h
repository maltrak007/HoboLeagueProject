// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumableStatTable.h"
#include "HoboLeagueProject/UI/HoboUserWidget.h"
#include "HHoldReleaseMinigame.generated.h"

class UImage;
class UOverlay;
class UHTierBox;

/**
 * Hold-Release Minigame Widget
 * Displays the minigame UI with tier boxes and moving indicator
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHHoldReleaseMinigame : public UHoboUserWidget
{
    GENERATED_BODY()

public:
    // ── Widget Components (Bind in UMG) ───────────────────────────

    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    UImage* TrackBackground;

    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    UOverlay* TierBoxesContainer;

    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    UImage* Indicator;

    // ── Configuration ─────────────────────────────────────────────

    UPROPERTY(EditDefaultsOnly, Category = "Minigame")
    TSubclassOf<UHTierBox> TierBoxWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "Minigame")
    float TrackWidth = 1520.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Minigame")
    float TrackHeight = 80.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Minigame")
    float MaxPosition = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Minigame")
    float MinPosition = 0.0f;

    // ── Runtime ───────────────────────────────────────────────────

    UPROPERTY()
    TArray<UHTierBox*> TierBoxes;

    // ── Public API ────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Minigame")
    void InitializeMinigame(
        const TArray<FConsumableMinigameTier>& Tiers,
        const TArray<FMinigameVisualZone>& VisualZones,
        float StartPosition
    );

    UFUNCTION(BlueprintCallable, Category = "Minigame")
    void UpdateIndicatorPosition(float Position);

    UFUNCTION(BlueprintCallable, Category = "Minigame")
    EMinigameTier GetTierAtPosition(float Position) const;

protected:
    virtual void NativeConstruct() override;

private:
    void CreateTierBoxes(
        const TArray<FConsumableMinigameTier>& Tiers,
        const TArray<FMinigameVisualZone>& VisualZones
    );
    
    void CreateZoneBoxes(const TArray<FMinigameVisualZone>& Zones, const TArray<FConsumableMinigameTier>& Tiers);
    void CreateDefaultTierBoxes(const TArray<FConsumableMinigameTier>& Tiers);
    
    void PositionTierBox(UHTierBox* TierBox, float StartPercent, float EndPercent);
    FLinearColor GetTierColor(EMinigameTier Tier, const TArray<FConsumableMinigameTier>& Tiers) const;
};