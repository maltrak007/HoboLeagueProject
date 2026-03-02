// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_UseConsumable.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Blueprint/UserWidget.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "HoboLeagueProject/GAS/GameplayAbility/Ability/AbilityTask/HAbilityTask_HoldReleaseMinigame.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumable.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumableDataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HConsumableStatTable.h"

void UGA_UseConsumable::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    // Cache consumable data
    CacheData();
    
    if (!CachedConsumable || !CachedConsumableStats)
    {
        UE_LOG(LogTemp, Error, TEXT("GA_UseConsumable: No consumable or stats cached!"));
        K2_EndAbility();
        return;
    }

    // Play animation montage
    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo) && ConsumableMontage)
    {
        UAbilityTask_PlayMontageAndWait* PlayMontageTask =
            UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
                this, NAME_None, ConsumableMontage);
        
        PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_UseConsumable::K2_EndAbility);
        PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_UseConsumable::K2_EndAbility);
        PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_UseConsumable::K2_EndAbility);
        PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_UseConsumable::K2_EndAbility);
        PlayMontageTask->ReadyForActivation();
    }

    // Wait for consume event from animation notify
    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
            this,
            GetUseConsumableEventTag()
        );
        
        WaitEventTask->EventReceived.AddDynamic(this, &UGA_UseConsumable::OnConsumableEventReceived);
        WaitEventTask->ReadyForActivation();
    }
}

void UGA_UseConsumable::InputReleased(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    // Forward input release to minigame task
    if (MinigameTask)
    {
        MinigameTask->OnInputReleased();
    }
}

void UGA_UseConsumable::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    ClearCacheData();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FGameplayTag UGA_UseConsumable::GetUseConsumableEventTag()
{
    return FGameplayTag::RequestGameplayTag(
        FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Consume)
    );
}

void UGA_UseConsumable::CacheData()
{
    APlayerCharacter* PC = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
    if (!PC) return;

    CachedConsumable = Cast<AHConsumable>(PC->GetInventoryComponent()->GetActiveConsumable());
    
    if (CachedConsumable)
    {
        CachedConsumableStats = CachedConsumable->GetConsumableStats();
    }
}

void UGA_UseConsumable::ClearCacheData()
{
    CachedConsumable = nullptr;
    CachedConsumableStats = nullptr;
    MinigameTask = nullptr;
    MinigameScore = 0.0f;
}

void UGA_UseConsumable::OnConsumableEventReceived(FGameplayEventData Data)
{
    if (!K2_HasAuthority()) return;

    // Check if consumable needs minigame
    if (CachedConsumableStats->bNeedScoreByTier)
    {
        StartMinigame();
    }
    else
    {
        // No minigame - apply effects immediately
        MinigameScore = 100.0f; // Full score
        ApplyConsumableEffects();
    }
}

void UGA_UseConsumable::StartMinigame()
{
    if (!CachedConsumableStats) return;

    // Check minigame type
    switch (CachedConsumableStats->MinigameType)
    {
        case EMinigameType::Hold_Release:
        {
            if (!HoldReleaseWidgetClass)
            {
                UE_LOG(LogTemp, Error, TEXT("GA_UseConsumable: HoldReleaseWidgetClass not set!"));
                MinigameScore = 0.0f;
                ApplyConsumableEffects();
                return;
            }

            // Create minigame task
            MinigameTask = UHAbilityTask_HoldReleaseMinigame::PlayHoldReleaseMinigame(
                this,
                HoldReleaseWidgetClass,
                *CachedConsumableStats,
                CachedConsumable->ItemDurability
            );

            if (MinigameTask)
            {
                MinigameTask->OnCompleted.AddDynamic(this, &UGA_UseConsumable::OnMinigameComplete);
                MinigameTask->OnCancelled.AddDynamic(this, &UGA_UseConsumable::OnMinigameCancelled);
                MinigameTask->ReadyForActivation();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("GA_UseConsumable: Failed to create minigame task!"));
                MinigameScore = 0.0f;
                ApplyConsumableEffects();
            }
            break;
        }
        
        case EMinigameType::Hold_Press:
        case EMinigameType::Hold:
        default:
        {
            UE_LOG(LogTemp, Warning, TEXT("GA_UseConsumable: Minigame type not yet implemented!"));
            MinigameScore = 100.0f;
            ApplyConsumableEffects();
            break;
        }
    }
}

void UGA_UseConsumable::OnMinigameComplete(float FinalScore)
{
    MinigameScore = FinalScore;
    ApplyConsumableEffects();
}

void UGA_UseConsumable::OnMinigameCancelled(float FinalScore)
{
    UE_LOG(LogTemp, Warning, TEXT("GA_UseConsumable: Minigame cancelled!"));
    MinigameScore = 0.0f;
    ApplyConsumableEffects();
}

void UGA_UseConsumable::ApplyConsumableEffects()
{
    if (!K2_HasAuthority() || !CachedConsumable || !CachedConsumableStats)
    {
        return;
    }

    const int32 AbilityLevel = GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;

    // Get multiplier from score
    float Multiplier = CachedConsumableStats->bNeedScoreByTier 
        ? CachedConsumableStats->GetMultiplierForScore(MinigameScore)
        : 1.0f;

    // Get tier for UI feedback
    EMinigameTier Tier = CachedConsumableStats->bNeedScoreByTier
        ? CachedConsumableStats->GetTierForScore(MinigameScore)
        : EMinigameTier::Perfect;

    FString TierName = UEnum::GetValueAsString(Tier);
    UE_LOG(LogTemp, Log, TEXT("Consumable Result: Score %.1f → %s → %.2fx multiplier"),
        MinigameScore, *TierName, Multiplier);

    // ── Apply Instant Effects ────────────────────────────────────

    if (CachedConsumable->GetItemConsumableDataAsset()->GetInstantGameplayEffect())
    {
        FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
            CachedConsumable->GetItemConsumableDataAsset()->GetInstantGameplayEffect(),
            AbilityLevel
        );
        
        if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
        {
            for (const FConsumableAttributeModifier& Mod : CachedConsumableStats->AttributeModifiers)
            {
                if (Mod.SetByCallerTag.IsValid())
                {
                    float FinalValue = Mod.BaseMagnitude * Multiplier;
                    
                    SpecHandle.Data->SetSetByCallerMagnitude(
                        Mod.SetByCallerTag,
                        FinalValue
                    );
                    
                    UE_LOG(LogTemp, Log, TEXT("  %s: %.1f × %.2f = %.1f"),
                        *Mod.DisplayName, Mod.BaseMagnitude, Multiplier, FinalValue);
                }
            }
            
            ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }

    // ── Apply Duration Effects ───────────────────────────────────

    if (CachedConsumable->GetItemConsumableDataAsset()->GetDurationGameplayEffect())
    {
        FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
            CachedConsumable->GetItemConsumableDataAsset()->GetDurationGameplayEffect(),
            AbilityLevel
        );
        
        if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
        {
            for (const FConsumableAttributeModifier& Mod : CachedConsumableStats->AttributeModifiers)
            {
                if (Mod.SetByCallerTag.IsValid())
                {
                    float FinalValue = Mod.BaseMagnitude * Multiplier;
                    
                    SpecHandle.Data->SetSetByCallerMagnitude(
                        Mod.SetByCallerTag,
                        FinalValue
                    );
                }
            }
            
            ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
    
    // ── Reduce Durability ────────────────────────────────────────

    CachedConsumable->ReduceDurability(CachedConsumableStats->DurabilityCostPeUse);
    
    // ── Broadcast Event ──────────────────────────────────────────

    CachedConsumable->OnConsumableUsed.Broadcast();
}