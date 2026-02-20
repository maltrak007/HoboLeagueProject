// ═══════════════════════════════════════════════════════════════════
// HConsumableStatsTable.h
// DataTable for consumable stats (minigame config + effect magnitudes)
// Lookup key: "{ConsumableID}_{Rarity}" e.g. "Syringe_Common", "Syringe_Rare"
// ═══════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "HoboLeagueProject/Item/HItemType.h"
#include "HConsumableStatTable.generated.h"

// ─────────────────────────────────────────────────────────────────
// One attribute modifier for consumables
// Maps SetByCaller tag → base magnitude (before skill check multiplier)
// ─────────────────────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FConsumableAttributeModifier
{
    GENERATED_BODY()

    // Friendly label (e.g. "Health Restore", "Stamina Boost")
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier")
    FString DisplayName = TEXT("Health Restore");

    // SetByCaller tag the GameplayEffect expects
    // e.g. "Data.Consumable.HealthBonus"
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier")
    FGameplayTag SetByCallerTag;

    // Base value BEFORE skill check multiplier
    // FinalValue = BaseMagnitude * (skill check tier multiplier)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier",
        meta=(ClampMin="0.0"))
    float BaseMagnitude = 50.0f;
};

// ─────────────────────────────────────────────────────────────────
// Skill check performance tier
// Maps score range → multiplier + visual feedback
// ─────────────────────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FConsumableMinigameTier
{
    GENERATED_BODY()

    // Display name (e.g. "Perfect", "Good", "Failed")
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tier")
    FName TierName = FName("Good");

    // Minimum score to reach this tier (0-100)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tier",
        meta=(ClampMin="0.0", ClampMax="100.0"))
    float MinScore = 50.0f;

    // Multiplier applied to all attribute modifiers
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tier",
        meta=(ClampMin="0.0", ClampMax="5.0"))
    float Multiplier = 1.0f;

    // Color for UI feedback
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tier")
    FLinearColor TierColor = FLinearColor(1.0f, 0.8f, 0.0f, 1.0f);
};

// ─────────────────────────────────────────────────────────────────
// Consumable stats row - one per (ConsumableType × Rarity)
// Row name format: "{ConsumableID}_{Rarity}"
// Examples: "Syringe_Common", "Syringe_Rare", "HealthPack_Epic"
// ─────────────────────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct HOBOLEAGUEPROJECT_API FConsumableStatsRow : public FTableRowBase
{
    GENERATED_BODY()

    // ── Effect Magnitudes ─────────────────────────────────────────

    // What attributes this consumable modifies + their base values
    // These get multiplied by the skill check tier multiplier
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TArray<FConsumableAttributeModifier> AttributeModifiers;

    // ── Minigame Configuration ────────────────────────────────────

    // How long the skill check minigame lasts
    // Higher rarities might have longer/shorter windows
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame",
        meta=(ClampMin="1.0", ClampMax="30.0", Units="s"))
    float MinigameDuration = 10.0f;

    // Score gained per second while holding in zone
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame",
        meta=(ClampMin="1.0"))
    float ScoreGainPerSecond = 10.0f;

    // Score lost per second while outside zone
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame",
        meta=(ClampMin="0.0"))
    float ScoreLossPerSecond = 5.0f;

    // Zone width as % of track (smaller = harder)
    // Rare items might have smaller zones for more challenge
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame",
        meta=(ClampMin="5.0", ClampMax="80.0"))
    float ZoneWidthPercent = 30.0f;

    // Indicator movement speed (% per second)
    // Rare items might move faster
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame",
        meta=(ClampMin="5.0", ClampMax="100.0"))
    float IndicatorSpeed = 35.0f;

    // ── Skill Check Tiers ─────────────────────────────────────────

    // Performance tiers - MUST be sorted by MinScore ASCENDING
    // Typically: Failed(0, 0.1x), Poor(30, 0.5x), Good(50, 1.0x), 
    //            Great(70, 1.5x), Perfect(90, 2.0x)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame Tiers")
    TArray<FConsumableMinigameTier> SkillCheckTiers;

    // ── Helpers ───────────────────────────────────────────────────

    // Get the multiplier for a given score
    float GetMultiplierForScore(float Score) const
    {
        float BestMultiplier = 0.1f; // Minimum fallback
        for (const FConsumableMinigameTier& Tier : SkillCheckTiers)
        {
            if (Score >= Tier.MinScore)
            {
                BestMultiplier = Tier.Multiplier;
            }
        }
        return BestMultiplier;
    }

    // Get tier name for UI display
    FName GetTierNameForScore(float Score) const
    {
        FName BestName = FName("Failed");
        for (const FConsumableMinigameTier& Tier : SkillCheckTiers)
        {
            if (Score >= Tier.MinScore)
            {
                BestName = Tier.TierName;
            }
        }
        return BestName;
    }

    // Get tier color for UI
    FLinearColor GetTierColorForScore(float Score) const
    {
        FLinearColor BestColor = FLinearColor::Red;
        for (const FConsumableMinigameTier& Tier : SkillCheckTiers)
        {
            if (Score >= Tier.MinScore)
            {
                BestColor = Tier.TierColor;
            }
        }
        return BestColor;
    }

    bool IsValid() const
    {
        return AttributeModifiers.Num() > 0
            && SkillCheckTiers.Num() > 0
            && MinigameDuration > 0.0f;
    }

#if WITH_EDITOR
    virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
    {
        Super::OnDataTableChanged(InDataTable, InRowName);

        // Validate tier sorting
        for (int32 i = 1; i < SkillCheckTiers.Num(); i++)
        {
            if (SkillCheckTiers[i].MinScore <= SkillCheckTiers[i - 1].MinScore)
            {
                UE_LOG(LogTemp, Warning,
                    TEXT("[DataTable '%s' Row '%s'] SkillCheckTiers not sorted! "
                         "Tier[%d] '%s' (%.0f) must be > Tier[%d] '%s' (%.0f)."),
                    *GetNameSafe(InDataTable), *InRowName.ToString(),
                    i,   *SkillCheckTiers[i].TierName.ToString(),   SkillCheckTiers[i].MinScore,
                    i-1, *SkillCheckTiers[i-1].TierName.ToString(), SkillCheckTiers[i-1].MinScore);
                break;
            }
        }
    }
#endif
};