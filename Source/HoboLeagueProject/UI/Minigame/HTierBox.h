// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumableStatTable.h"
#include "HoboLeagueProject/UI/HoboUserWidget.h"
#include "HTierBox.generated.h"

/**
 * Visual representation of a single tier zone in the minigame
 * Displays a colored box representing a score tier (Bad, Average, Great, Perfect)
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHTierBox : public UHoboUserWidget
{
    GENERATED_BODY()

public:
    // ── Tier Data ─────────────────────────────────────────────────

    /** Configuration for this tier (score range, multiplier, color) */
    UPROPERTY(BlueprintReadWrite, Category = "Tier")
    FConsumableMinigameTier TierData;

    /** End score for this tier (exclusive upper bound) */
    UPROPERTY(BlueprintReadOnly, Category = "Tier")
    float TierEndScore = 100.0f;

    // ── Visual Components ─────────────────────────────────────────

    /** Background image colored based on tier */
    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    UImage* Background;

    // ── Public API ────────────────────────────────────────────────
    virtual void NativeConstruct() override;
    /**
     * Initialize this tier box with data
     * @param InTierData - Tier configuration (score, multiplier, color)
     * @param EndScore - Upper bound of this tier's score range
     */
    UFUNCTION(BlueprintCallable, Category = "Tier")
    void InitializeTier(const FConsumableMinigameTier& InTierData, float EndScore = 100.0f);

    /**
     * Check if a score position falls within this tier's range
     * @param Position - Normalized score position (0-100%)
     * @return True if Position is within [MinScore, EndScore)
     */
    UFUNCTION(BlueprintCallable, Category = "Tier")
    bool IsPositionInTier(float Position) const
    {
        return Position >= TierData.MinScore && Position < TierEndScore;
    }
};