// ═══════════════════════════════════════════════════════════════════
// HWeaponStatsTable.h
// DataTable for weapon stats (damage, durability, attack speed)
// Lookup key: "{WeaponID}_{Rarity}" e.g. "Sword_Common", "Sword_Rare"
// ═══════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "HWeaponStatsTable.generated.h"

// ─────────────────────────────────────────────────────────────────
// One damage modifier entry for weapons
// Used to configure damage GameplayEffects via SetByCaller
// ─────────────────────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FWeaponDamageModifier
{
    GENERATED_BODY()

    // Friendly label (e.g. "Base Damage", "Critical Damage")
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    FString DisplayName = TEXT("Base Damage");

    // SetByCaller tag the damage GE expects
    // e.g. "Data.Damage.Base", "Data.Damage.Critical"
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    FGameplayTag SetByCallerTag;

    // Base damage value for this rarity tier
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    float BaseDamage = 10.0f;
};

// ─────────────────────────────────────────────────────────────────
// Weapon stats row - one per (WeaponType × Rarity) combination
// Row name format: "{WeaponID}_{Rarity}"
// Examples: "Sword_Common", "Sword_Rare", "Hammer_Epic"
// ─────────────────────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct HOBOLEAGUEPROJECT_API FWeaponStatsRow : public FTableRowBase
{
    GENERATED_BODY()

    // ── Combat Stats ──────────────────────────────────────────────

    // Damage modifiers (supports multiple damage types per weapon)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TArray<FWeaponDamageModifier> DamageModifiers;

    // Total durability for this rarity
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat",
        meta=(ClampMin="1.0"))
    float MaxDurability = 100.0f;

    // Durability cost per hit
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat",
        meta=(ClampMin="0.0"))
    float DurabilityCostPerHit = 1.0f;

    // Attack speed multiplier (1.0 = normal, 1.5 = 50% faster)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat",
        meta=(ClampMin="0.1", ClampMax="5.0"))
    float AttackSpeedMultiplier = 1.0f;

    // ── Stamina Costs ─────────────────────────────────────────────

    // Stamina cost for light attack
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
    float LightAttackStaminaCost = 10.0f;

    // Stamina cost for heavy attack
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
    float ChargedAttackStaminaCost = 25.0f;

    // ── Optional Bonuses ──────────────────────────────────────────

    // Bonus critical hit chance (0.0 - 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonuses",
        meta=(ClampMin="0.0", ClampMax="1.0"))
    float CriticalChanceBonus = 0.0f;

    // Bonus critical damage multiplier
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonuses",
        meta=(ClampMin="1.0", ClampMax="10.0"))
    float CriticalDamageMultiplier = 1.5f;

    // ── Helpers ───────────────────────────────────────────────────

    // Get damage value for a specific tag
    float GetDamageForTag(const FGameplayTag& Tag) const
    {
        for (const FWeaponDamageModifier& Modifier : DamageModifiers)
        {
            if (Modifier.SetByCallerTag.MatchesTagExact(Tag))
            {
                return Modifier.BaseDamage;
            }
        }
        return 0.0f;
    }

    bool IsValid() const
    {
        return DamageModifiers.Num() > 0 && MaxDurability > 0.0f;
    }
};