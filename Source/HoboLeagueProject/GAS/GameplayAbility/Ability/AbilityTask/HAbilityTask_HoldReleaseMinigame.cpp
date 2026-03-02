// Fill out your copyright notice in the Description page of Project Settings.

#include "HAbilityTask_HoldReleaseMinigame.h"
#include "HoboLeagueProject/UI/Minigame/HHoldReleaseMinigame.h"
#include "Blueprint/UserWidget.h"

UHAbilityTask_HoldReleaseMinigame* UHAbilityTask_HoldReleaseMinigame::PlayHoldReleaseMinigame(
    UGameplayAbility* OwningAbility,
    TSubclassOf<UUserWidget> WidgetClass,
    const FConsumableStatsRow& ConsumableStats,
    float StartPosition)
{
    UHAbilityTask_HoldReleaseMinigame* Task = NewAbilityTask<UHAbilityTask_HoldReleaseMinigame>(OwningAbility);
    
    Task->MinigameWidgetClass = WidgetClass;
    Task->ConsumableConfig = ConsumableStats;
    Task->StartPos = StartPosition;
    Task->EndPos = 0.0f;
    
    return Task;
}

void UHAbilityTask_HoldReleaseMinigame::Activate()
{
    Super::Activate();
    
    if (!MinigameWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("HoldReleaseMinigame: WidgetClass is null!"));
        EndTask();
        return;
    }
    
    // Create widget
    MinigameWidget = CreateWidget<UHHoldReleaseMinigame>(GetWorld(), MinigameWidgetClass);
    if (MinigameWidget)
    {
        // Initialize widget with config
        MinigameWidget->InitializeMinigame(
            ConsumableConfig.SkillCheckTiers,
            ConsumableConfig.HoldReleaseConfig.VisualSkillCheckTiers,
            StartPos
        );
        
        MinigameWidget->AddToViewport(100);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HoldReleaseMinigame: Failed to create widget!"));
        EndTask();
        return;
    }
    
    // Start minigame state
    CurrentPosition = StartPos;
    bIsHolding = true;
    bIsSliding = false;
    
    // Enable ticking
    bTickingTask = true;
}

void UHAbilityTask_HoldReleaseMinigame::OnDestroy(bool bInOwnerFinished)
{
    if (MinigameWidget && MinigameWidget->IsInViewport())
    {
        MinigameWidget->RemoveFromParent();
    }
    
    bTickingTask = false;
    
    Super::OnDestroy(bInOwnerFinished);
}

void UHAbilityTask_HoldReleaseMinigame::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);
    
    if (bIsHolding)
    {
        UpdateIndicator(DeltaTime);
    }
    else if (bIsSliding)
    {
        UpdateSlide(DeltaTime);
    }
}

void UHAbilityTask_HoldReleaseMinigame::UpdateIndicator(float DeltaTime)
{
    float Speed = ConsumableConfig.HoldReleaseConfig.IndicatorSpeed;
    
    CurrentPosition -= Speed * DeltaTime;
    CurrentPosition = FMath::Clamp(CurrentPosition, EndPos, StartPos);
    
    if (MinigameWidget)
    {
        MinigameWidget->UpdateIndicatorPosition(CurrentPosition);
    }
    
    if (CurrentPosition <= EndPos)
    {
        bIsHolding = false;
        StartSlide();
    }
}

void UHAbilityTask_HoldReleaseMinigame::OnInputReleased()
{
    if (bIsHolding)
    {
        UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════"));
        UE_LOG(LogTemp, Warning, TEXT("INPUT RELEASED - Starting Slide"));
        UE_LOG(LogTemp, Warning, TEXT("  Current Position: %.2f%%"), CurrentPosition);
        UE_LOG(LogTemp, Warning, TEXT("  Start Position: %.2f%%"), StartPos);
        UE_LOG(LogTemp, Warning, TEXT("  Slide Distance: %.2f%%"), SlideDistance);
        UE_LOG(LogTemp, Warning, TEXT("  Expected End: %.2f%%"), CurrentPosition - SlideDistance);
        UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════"));
        bIsHolding = false;
        StartSlide();
    }
}

void UHAbilityTask_HoldReleaseMinigame::StartSlide()
{
    bIsSliding = true;
    SlideStartPosition = CurrentPosition;
    SlideElapsedTime = 0.0f;
}

void UHAbilityTask_HoldReleaseMinigame::UpdateSlide(float DeltaTime)
{
    SlideElapsedTime += DeltaTime;
    
    float Alpha = FMath::Clamp(SlideElapsedTime / SlideDuration, 0.0f, 1.0f);
    Alpha = 1.0f - FMath::Pow(1.0f - Alpha, 3.0f);
    
    CurrentPosition = SlideStartPosition - (SlideDistance * Alpha);
    CurrentPosition = FMath::Clamp(CurrentPosition, EndPos, StartPos);
    
    if (MinigameWidget)
    {
        MinigameWidget->UpdateIndicatorPosition(CurrentPosition);
    }
    
    if (SlideElapsedTime >= SlideDuration)
    {
        bIsSliding = false;
        CompleteMinigame();
    }
}

float UHAbilityTask_HoldReleaseMinigame::CalculateFinalScore() const
{
    float NormalizedPosition = (CurrentPosition / StartPos) * 100.0f;
    return FMath::Clamp(NormalizedPosition, 0.0f, 100.0f);
}

void UHAbilityTask_HoldReleaseMinigame::CompleteMinigame()
{
    float FinalScore = CalculateFinalScore();
    
    UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════"));
    UE_LOG(LogTemp, Warning, TEXT("MINIGAME COMPLETE"));
    UE_LOG(LogTemp, Warning, TEXT("  Final Position: %.2f%% (raw)"), CurrentPosition);
    UE_LOG(LogTemp, Warning, TEXT("  Start Position: %.2f%%"), StartPos);
    UE_LOG(LogTemp, Warning, TEXT("  Normalized Score: %.2f%% (0-100 scale)"), FinalScore);
    UE_LOG(LogTemp, Warning, TEXT("  Formula: (%.2f / %.2f) × 100 = %.2f"), 
        CurrentPosition, StartPos, FinalScore);
    UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════"));
    
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnCompleted.Broadcast(FinalScore);
    }
    
    EndTask();
}

void UHAbilityTask_HoldReleaseMinigame::CancelMinigame()
{
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnCancelled.Broadcast(0.0f);
    }
    
    EndTask();
}