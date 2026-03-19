// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/Item/NonPlayerItem/HNonPlayerItem.h"
#include "HItemGenerator.generated.h"

enum class ERarityType : uint8;
class AHPlayerItem;

/**
 * Item configuration data
 */
USTRUCT(BlueprintType)
struct FItemSpawnData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AHPlayerItem> ItemClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName ItemID;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float SpawnWeight = 1.0f;
};

/**
 * Item generator that spawns and launches items on interaction
 */
UCLASS()
class HOBOLEAGUEPROJECT_API AHItemGenerator : public AHNonPlayerItem
{
    GENERATED_BODY()

public:
    AHItemGenerator();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;
    
    virtual void Interact_Implementation(APlayerCharacter* PlayerOwner) override;
    
    // ══════════════════════════════════════════════════════════════
    // CONFIGURATION
    // ══════════════════════════════════════════════════════════════

    /** Item spawn configurations */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
    TArray<FItemSpawnData> ItemSpawnData;

    /** Rarity chances (percentage 0-100) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
    TMap<ERarityType, float> RaritySpawnChances;
    
    // ══════════════════════════════════════════════════════════════
    // COOLDOWN
    // ══════════════════════════════════════════════════════════════

    UPROPERTY(EditDefaultsOnly, Category = "Cooldown", meta = (ClampMin = "0.0"))
    float CooldownDuration = 60.0f;

    /** Replicated cooldown remaining */
    UPROPERTY(ReplicatedUsing = OnRep_IsOnCooldown, BlueprintReadOnly, Category = "Cooldown")
    bool bIsOnCooldown = false;

    UFUNCTION()
    void OnRep_IsOnCooldown();

    // ══════════════════════════════════════════════════════════════
    // DEBUG
    // ══════════════════════════════════════════════════════════════

    UPROPERTY(EditDefaultsOnly, Category = "Debug")
    bool bDrawDebug = false;

private:
    // ══════════════════════════════════════════════════════════════
    // NETWORKING
    // ══════════════════════════════════════════════════════════════

    /** Server RPC for item generation request */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRequestGenerate(APlayerCharacter* Player);
    
    // ══════════════════════════════════════════════════════════════
    // ITEM GENERATION
    // ══════════════════════════════════════════════════════════════

    /** Select random item ID based on weights */
    FName GetRandomItemID() const;

    /** Select random rarity based on percentages */
    ERarityType GetRandomRarity() const;

    /** Find item class by ID */
    const FItemSpawnData* FindItemDataByID(FName ItemID) const;

    /** Spawn and configure item */
    void GenerateItem(FName ItemID, ERarityType Rarity);

    // ══════════════════════════════════════════════════════════════
    // COOLDOWN MANAGEMENT
    // ══════════════════════════════════════════════════════════════

    FTimerHandle CooldownTimerHandle;

    void StartCooldown();
    
    void OnCooldownComplete();
};