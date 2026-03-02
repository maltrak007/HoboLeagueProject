// ═══════════════════════════════════════════════════════════════════
// HConsumableStatsTable.h
// DataTable for consumable stats (minigame config + effect magnitudes)
// Lookup key: "{ConsumableID}_{Rarity}" e.g. "Syringe_Common", "Syringe_Rare"
// ═══════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
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
        meta=(ClampMin="-100.0",ClampMax="100.0"))
    float BaseMagnitude = 50.0f;
};

UENUM(BlueprintType)
enum class EMinigameTier : uint8
{
    Perfect		UMETA(DisplayName = "Perfect Score"),
    Great	    UMETA(DisplayName = "Great Score"),
    Average     UMETA(DisplayName = "Average Score"),
    Bad         UMETA(DisplayName = "Bad Score")
};

// ─────────────────────────────────────────────────────────────────
// Skill check performance tier
// Maps score range → multiplier + visual feedback
// ─────────────────────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FConsumableMinigameTier
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tier")
    EMinigameTier TierSelected = EMinigameTier::Average;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tier",
        meta=(ClampMin="0.0", ClampMax="100.0"))
    float MinScore = 50.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tier",
        meta=(ClampMin="-5.0", ClampMax="5.0"))
    float Multiplier = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tier")
    FLinearColor TierColor = FLinearColor(1.0f, 0.8f, 0.0f, 1.0f);
};

USTRUCT(BlueprintType)
struct FMinigameVisualZone
{
    GENERATED_BODY()
    
    // Which tier this zone represents
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    EMinigameTier TierType = EMinigameTier::Average;
    
    // Visual position on track (0-100% on full scale)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone",
        meta=(ClampMin="0.0", ClampMax="100.0"))
    float StartPercent = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone",
        meta=(ClampMin="0.0", ClampMax="100.0"))
    float EndPercent = 20.0f;
    
    // Color override (optional - can use tier's default color)
    // UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    // bool bOverrideColor = false;
    //
    // UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone",
    //     meta=(EditCondition="bOverrideColor"))
    // FLinearColor CustomColor = FLinearColor::White;
};


UENUM(BlueprintType)
enum class EMinigameType : uint8
{
    Hold_Release		UMETA(DisplayName = "Hold and release in zone"),
    Hold_Press			UMETA(DisplayName = "Hold and press in zone"),
    Hold                UMETA(DisplayName = "Hold in zone")
};

USTRUCT(BlueprintType)
struct FHoldReleaseConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hold Release")
    TArray<FMinigameVisualZone> VisualSkillCheckTiers;
    
    // Indicator movement speed (% of track per second)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hold Release",
        meta=(ClampMin="5.0", ClampMax="100.0"))
    float IndicatorSpeed = 35.0f;
};

// ─────────────────────────────────────────────────────────────────
// Tap Timing Minigame Config
// Player must tap at exact moment indicator hits zone
// ─────────────────────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FHoldPressConfig
{
    GENERATED_BODY()

    // Number of taps required
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing",
        meta=(ClampMin="1", ClampMax="10"))
    int32 RequiredTaps = 3;

    // Indicator movement speed (% of track per second)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing",
        meta=(ClampMin="10.0", ClampMax="150.0"))
    float IndicatorSpeed = 50.0f;

    // Perfect zone width (green) - center of target
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing",
        meta=(ClampMin="1.0", ClampMax="20.0"))
    float PerfectZonePercent = 5.0f;

    // Good zone width (yellow) - around perfect
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing",
        meta=(ClampMin="5.0", ClampMax="40.0"))
    float GoodZonePercent = 15.0f;

    // Okay zone width (orange) - around good
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing",
        meta=(ClampMin="10.0", ClampMax="60.0"))
    float OkayZonePercent = 30.0f;

    // Score per tap tier
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing")
    float PerfectTapScore = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing")
    float GoodTapScore = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing")
    float OkayTapScore = 10.0f;

    // Penalty for missed tap
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing")
    float MissPenalty = -15.0f;

    // Time window to tap after indicator passes (in seconds)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tap Timing",
        meta=(ClampMin="0.1", ClampMax="2.0", Units="s"))
    float TapWindow = 0.5f;
};

// ─────────────────────────────────────────────────────────────────
// Smash Tap Minigame Config
// Player rapidly taps button to build score
// ─────────────────────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FHoldConfig
{
    GENERATED_BODY()

    // Total duration to smash
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash Tap",
        meta=(ClampMin="1.0", ClampMax="30.0", Units="s"))
    float Duration = 5.0f;

    // Score gained per tap
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash Tap",
        meta=(ClampMin="0.1"))
    float ScorePerTap = 2.0f;

    // Target taps per second for 100% score
    // Example: 10 TPS × 5 seconds = 50 total taps for 100 score
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash Tap",
        meta=(ClampMin="1.0", ClampMax="20.0"))
    float TargetTapsPerSecond = 10.0f;

    // Whether score decays over time if not tapping
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash Tap")
    bool bScoreDecays = false;

    // Score decay rate per second (if bScoreDecays = true)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash Tap",
        meta=(ClampMin="0.0", EditCondition="bScoreDecays"))
    float DecayRatePerSecond = 1.0f;

    // Maximum score achievable (prevents infinite tapping)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash Tap",
        meta=(ClampMin="50.0"))
    float MaxScore = 100.0f;

    // Visual feedback: Show taps per second indicator
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash Tap")
    bool bShowTPSIndicator = true;

    // Whether indicator bounces or wraps around
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hold Release")
    bool bBouncesAtEdges = true;
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
    
    // For example the cigarette timer and skill tiers width is determined by the amount left, but in the syringe I can specify a timer because it's only one skill check
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame")
    bool bHasOwnDuration = false;

    // // How long the skill check minigame lasts
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame",
    meta=(ClampMin="1.0", ClampMax="30.0", Units="s"),meta=(EditCondition="bHasOwnDuration"))
    float MinigameDuration = 10.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame")
    bool bNeedScoreByTier = false;
    
    // ══════════════════════════════════════════════════════════════
    // SKILL CHECK TIERS (Common to all minigames)
    // ══════════════════════════════════════════════════════════════
    // Performance tiers - MUST be sorted by MinScore ASCENDING
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Tiers", meta=(EditCondition="bNeedScoreByTier"))
    TArray<FConsumableMinigameTier> SkillCheckTiers;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data",
    meta=(ClampMin="1.0", ClampMax="100.0"))
    float MaxConsumableDurability = 10.0f;

    // Durability cost per hit
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data",meta=(ClampMin="-100.0", ClampMax="100.0"))
    float DurabilityCostPeUse = -10.0f;
    
    // Which type of minigame this consumable uses
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame")
    EMinigameType MinigameType = EMinigameType::Hold;

    // ══════════════════════════════════════════════════════════════
    // MINIGAME-SPECIFIC CONFIGURATIONS
    // Only the config matching MinigameType is used
    // ══════════════════════════════════════════════════════════════
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Hold In Zone",
        meta=(EditCondition="MinigameType == EMinigameType::Hold_Release", EditConditionHides))
    FHoldReleaseConfig HoldReleaseConfig;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Tap Timing",
        meta=(EditCondition="MinigameType == EMinigameType::Hold_Press", EditConditionHides))
    FHoldPressConfig HoldPressConfig;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Smash Tap",
        meta=(EditCondition="MinigameType == EMinigameType::Hold", EditConditionHides))
    FHoldConfig HoldConfig;
    
    // ── Helpers ───────────────────────────────────────────────────

    // Get the multiplier for a given score
    float GetMultiplierForScore(float Score) const
    {
        float BestMultiplier = 1.f; // Minimum fallback
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
    EMinigameTier GetTierForScore(float Score) const
    {
        EMinigameTier BestTier = EMinigameTier::Bad;
        for (const FConsumableMinigameTier& Tier : SkillCheckTiers)
        {
            if (Score >= Tier.MinScore)
            {
                BestTier = Tier.TierSelected;
            }
        }
        return BestTier;
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
            && SkillCheckTiers.Num() > 0;
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
                FString TierName = UEnum::GetValueAsString(SkillCheckTiers[i].TierSelected);
                FString PrevTierName = UEnum::GetValueAsString(SkillCheckTiers[i-1].TierSelected);
            
                UE_LOG(LogTemp, Warning,
                    TEXT("[DataTable '%s' Row '%s'] SkillCheckTiers not sorted! "
                         "Tier[%d] '%s' (%.0f) must be > Tier[%d] '%s' (%.0f)."),
                    *GetNameSafe(InDataTable), *InRowName.ToString(),
                    i,   *TierName,   SkillCheckTiers[i].MinScore,
                    i-1, *PrevTierName, SkillCheckTiers[i-1].MinScore);
                break;
            }
        }
    }
#endif
};