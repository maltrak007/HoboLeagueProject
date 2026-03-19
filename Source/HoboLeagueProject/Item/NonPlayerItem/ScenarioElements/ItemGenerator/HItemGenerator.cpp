// Fill out your copyright notice in the Description page of Project Settings.

#include "HItemGenerator.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInteractionComponent.h"

// ══════════════════════════════════════════════════════════════
// CONSTRUCTION
// ══════════════════════════════════════════════════════════════

AHItemGenerator::AHItemGenerator()
{
    PrimaryActorTick.bCanEverTick = false; 
    PrimaryActorTick.bStartWithTickEnabled = false;
    
    bReplicates = true;
}

void AHItemGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // Only replicate cooldown state, not time remaining (reduces bandwidth)
    DOREPLIFETIME(AHItemGenerator, bIsOnCooldown);
}

void AHItemGenerator::BeginPlay()
{
    Super::BeginPlay();
    
    // Validate configuration
    if (ItemSpawnData.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ItemGenerator] %s: No items configured!"), *GetName());
    }

    // Validate rarity chances add up reasonably
    if (HasAuthority())
    {
        float TotalRarity = 0.0f;
        for (const TPair<ERarityType, float>& Pair : RaritySpawnChances)
        {
            TotalRarity += Pair.Value;
        }
        
        if (!FMath::IsNearlyEqual(TotalRarity, 100.0f, 1.0f))
        {
            UE_LOG(LogTemp, Warning, TEXT("[ItemGenerator] %s: Rarity chances total %.2f (should be ~100)"), 
                *GetName(), TotalRarity);
        }
    }
}

// ══════════════════════════════════════════════════════════════
// INTERACTION
// ══════════════════════════════════════════════════════════════

void AHItemGenerator::Interact_Implementation(APlayerCharacter* PlayerOwner)
{
    UE_LOG(LogTemp, Verbose, TEXT("Trying to interact"));
    if (PlayerOwner)
    {
        OwningPlayer = PlayerOwner;
        
        OwningPlayer->GetInteractionComponent()->RemoveInteractableObject(this);
    }
    // Client-side validation
    if (bIsOnCooldown)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[ItemGenerator] On cooldown"));
        // Could trigger UI feedback here
        return;
    }

    // Request from server
    ServerRequestGenerate(PlayerOwner);
}

// ══════════════════════════════════════════════════════════════
// SERVER RPC
// ══════════════════════════════════════════════════════════════

bool AHItemGenerator::ServerRequestGenerate_Validate(APlayerCharacter* Player)
{
    return Player != nullptr;
}

void AHItemGenerator::ServerRequestGenerate_Implementation(APlayerCharacter* Player)
{
    // Server-side validation
    if (bIsOnCooldown)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ItemGenerator] Server rejected - on cooldown"));
        return;
    }
    
    // Generate item
    const FName ItemID = GetRandomItemID();
    if (ItemID.IsNone())
    {
        UE_LOG(LogTemp, Error, TEXT("[ItemGenerator] Failed to select item"));
        return;
    }

    const ERarityType Rarity = GetRandomRarity();

    // Spawn item
    GenerateItem(ItemID, Rarity);

    // Start cooldown
    StartCooldown();
}

// ══════════════════════════════════════════════════════════════
// RANDOM SELECTION
// ══════════════════════════════════════════════════════════════

FName AHItemGenerator::GetRandomItemID() const
{
    if (ItemSpawnData.Num() == 0)
    {
        return NAME_None;
    }

    // Calculate total weight
    float TotalWeight = 0.0f;
    for (const FItemSpawnData& Data : ItemSpawnData)
    {
        if (Data.SpawnWeight > 0.0f)
        {
            TotalWeight += Data.SpawnWeight;
        }
    }

    if (TotalWeight <= 0.0f)
    {
        return NAME_None;
    }

    // Weighted random selection
    const float Roll = FMath::FRandRange(0.0f, TotalWeight);
    float Cumulative = 0.0f;

    for (const FItemSpawnData& Data : ItemSpawnData)
    {
        if (Data.SpawnWeight > 0.0f)
        {
            Cumulative += Data.SpawnWeight;
            if (Roll <= Cumulative)
            {
                return Data.ItemID;
            }
        }
    }

    // Fallback
    return ItemSpawnData[0].ItemID;
}

ERarityType AHItemGenerator::GetRandomRarity() const
{
    if (RaritySpawnChances.Num() == 0)
    {
        return ERarityType::Common;
    }

    const float Roll = FMath::FRandRange(0.0f, 100.0f);
    float Cumulative = 0.0f;

    // Sort by rarity for consistent behavior
    TArray<TPair<ERarityType, float>> SortedRarities;
    for (const TPair<ERarityType, float>& Pair : RaritySpawnChances)
    {
        SortedRarities.Add(Pair);
    }
    
    // Sort descending by chance (highest first)
    SortedRarities.Sort([](const TPair<ERarityType, float>& A, const TPair<ERarityType, float>& B) {
        return A.Value > B.Value;
    });

    for (const TPair<ERarityType, float>& Pair : SortedRarities)
    {
        Cumulative += Pair.Value;
        if (Roll <= Cumulative)
        {
            return Pair.Key;
        }
    }

    return SortedRarities[0].Key;
}

const FItemSpawnData* AHItemGenerator::FindItemDataByID(FName ItemID) const
{
    for (const FItemSpawnData& Data : ItemSpawnData)
    {
        if (Data.ItemID == ItemID)
        {
            return &Data;
        }
    }
    return nullptr;
}

void AHItemGenerator::GenerateItem(FName ItemID, ERarityType Rarity)
{
    const FItemSpawnData* ItemData = FindItemDataByID(ItemID);
    if (!ItemData || !ItemData->ItemClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[ItemGenerator] No valid class for ID: %s"), *ItemID.ToString());
        return;
    }

    const FVector SpawnPosition = GetActorLocation() + FVector(100, 0, 50   ); // Spawn slightly above generator
    
    // Spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Deferred spawning for configuration before BeginPlay
    AHPlayerItem* SpawnedItem = GetWorld()->SpawnActorDeferred<AHPlayerItem>(
        ItemData->ItemClass,
        FTransform(FRotator::ZeroRotator, SpawnPosition),
        this,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    if (!SpawnedItem)
    {
        UE_LOG(LogTemp, Error, TEXT("[ItemGenerator] Failed to spawn item"));
        return;
    }
    
    SpawnedItem->ItemRarity = Rarity;

    // Disable collision initially
    // if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(SpawnedItem->GetRootComponent()))
    // {
    //     RootPrimitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //     RootPrimitive->SetSimulatePhysics(false);
    // }

    // Finish spawning
    SpawnedItem->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnPosition));
    

    UE_LOG(LogTemp, Log, TEXT("[ItemGenerator] Generated: %s (Rarity: %d)"), 
        *ItemID.ToString(), static_cast<int32>(Rarity));
}

// ══════════════════════════════════════════════════════════════
// COOLDOWN
// ══════════════════════════════════════════════════════════════

void AHItemGenerator::StartCooldown()
{
    bIsOnCooldown = true;

    GetWorld()->GetTimerManager().SetTimer(
        CooldownTimerHandle,
        this,
        &AHItemGenerator::OnCooldownComplete,
        CooldownDuration,
        false
    );

    UE_LOG(LogTemp, Log, TEXT("[ItemGenerator] Cooldown started: %.1fs"), CooldownDuration);
}

void AHItemGenerator::OnCooldownComplete()
{
    bIsOnCooldown = false;
    GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);

    UE_LOG(LogTemp, Log, TEXT("[ItemGenerator] Cooldown complete"));
}

void AHItemGenerator::OnRep_IsOnCooldown()
{
    // Update UI or visual feedback on clients
    UE_LOG(LogTemp, Verbose, TEXT("[ItemGenerator] Cooldown state: %s"), 
        bIsOnCooldown ? TEXT("Active") : TEXT("Ready"));
}