// Fill out your copyright notice in the Description page of Project Settings.

#include "HHoldReleaseMinigame.h"
#include "HTierBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

void UHHoldReleaseMinigame::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Get track dimensions from widget
    if (TrackBackground)
    {
        FVector2D Size = TrackBackground->GetDesiredSize();
        if (Size.X > 0.0f)
        {
            TrackWidth = Size.X;
            TrackHeight = Size.Y;
        }
    }
}

void UHHoldReleaseMinigame::InitializeMinigame(
    const TArray<FConsumableMinigameTier>& Tiers,
    const TArray<FMinigameVisualZone>& VisualZones,
    float StartPosition)
{
    MaxPosition = StartPosition;
    MinPosition = 0.0f;
    
    // Clear existing boxes
    if (TierBoxesContainer)
    {
        TierBoxesContainer->ClearChildren();
    }
    TierBoxes.Empty();
    
    // Create tier boxes
    CreateTierBoxes(Tiers, VisualZones);
}

void UHHoldReleaseMinigame::UpdateIndicatorPosition(float Position)
{
    if (!Indicator)
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateIndicatorPosition: Indicator is NULL!"));
        return;
    }

    // Normalize position to current range (0-MaxPosition → 0-1)
    float NormalizedPos = Position / MaxPosition;
    NormalizedPos = FMath::Clamp(NormalizedPos, 0.0f, 1.0f);
    
    // Convert to pixels along the track
    float PixelPosition = NormalizedPos * TrackWidth;
    
    // Get track's starting position on screen
    FVector2D TrackPosition = FVector2D(200.0f, 500.0f); // Default fallback
    
    if (TrackBackground)
    {
        UCanvasPanelSlot* TrackSlot = Cast<UCanvasPanelSlot>(TrackBackground->Slot);
        if (TrackSlot)
        {
            TrackPosition = TrackSlot->GetPosition();
        }
    }
    
    // Get TierBoxesContainer position (should match track)
    if (TierBoxesContainer)
    {
        UCanvasPanelSlot* ContainerSlot = Cast<UCanvasPanelSlot>(TierBoxesContainer->Slot);
        if (ContainerSlot)
        {
            TrackPosition = ContainerSlot->GetPosition(); // Use container position for accuracy
        }
    }
    
    // Update indicator position
    UCanvasPanelSlot* HSlot = Cast<UCanvasPanelSlot>(Indicator->Slot);
    if (HSlot)
    {
        // Position indicator: Track's X + pixel offset, Track's Y - 15 (above track)
        float IndicatorX = TrackPosition.X + PixelPosition;
        float IndicatorY = TrackPosition.Y - 15.0f; // 15 pixels above track
        
        HSlot->SetPosition(FVector2D(IndicatorX, IndicatorY));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateIndicatorPosition: Indicator Slot is NULL!"));
    }
}

void UHHoldReleaseMinigame::CreateTierBoxes(
    const TArray<FConsumableMinigameTier>& Tiers,
    const TArray<FMinigameVisualZone>& VisualZones)
{
    if (!TierBoxWidgetClass || !TierBoxesContainer)
    {
        UE_LOG(LogTemp, Error, TEXT("HHoldReleaseMinigame: TierBoxWidgetClass or TierBoxesContainer is null!"));
        return;
    }

    // Use custom zones if provided, otherwise auto-generate from tiers
    if (VisualZones.Num() > 0)
    {
        CreateZoneBoxes(VisualZones, Tiers);
    }
    else
    {
        CreateDefaultTierBoxes(Tiers);
    }
}

void UHHoldReleaseMinigame::CreateZoneBoxes(
    const TArray<FMinigameVisualZone>& Zones,
    const TArray<FConsumableMinigameTier>& Tiers)
{
    for (const FMinigameVisualZone& Zone : Zones)
    {
        UHTierBox* TierBox = CreateWidget<UHTierBox>(this, TierBoxWidgetClass);
        if (!TierBox) continue;
        
        // Find tier data for this zone
        const FConsumableMinigameTier* TierData = Tiers.FindByPredicate(
            [&Zone](const FConsumableMinigameTier& T) { return T.TierSelected == Zone.TierType; }
        );
        
        if (TierData)
        {
            TierBox->InitializeTier(*TierData, Zone.EndPercent);
        }
        
        // ✅ ADD TO CONTAINER FIRST
        TierBoxesContainer->AddChild(TierBox);
        TierBoxes.Add(TierBox);
        
        // ✅ THEN position (slot exists now)
        float ScaledStart = (Zone.StartPercent / 100.0f) * MaxPosition;
        float ScaledEnd = (Zone.EndPercent / 100.0f) * MaxPosition;
        
        PositionTierBox(TierBox, ScaledStart, ScaledEnd);
    }
}

void UHHoldReleaseMinigame::CreateDefaultTierBoxes(const TArray<FConsumableMinigameTier>& Tiers)
{
    UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════"));
    UE_LOG(LogTemp, Warning, TEXT("CREATING TIER BOXES"));
    UE_LOG(LogTemp, Warning, TEXT("  Max Position (Durability): %.1f%%"), MaxPosition);
    UE_LOG(LogTemp, Warning, TEXT("  Track Width: %.1f pixels"), TrackWidth);
    UE_LOG(LogTemp, Warning, TEXT("  Number of Tiers: %d"), Tiers.Num());
    UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════"));
    
    if (!TierBoxWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateDefaultTierBoxes: TierBoxWidgetClass is NULL!"));
        return;
    }
    
    if (!TierBoxesContainer)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateDefaultTierBoxes: TierBoxesContainer is NULL!"));
        return;
    }

    for (int32 i = 0; i < Tiers.Num(); i++)
    {
        const FConsumableMinigameTier& Tier = Tiers[i];
        
        UHTierBox* TierBox = CreateWidget<UHTierBox>(this, TierBoxWidgetClass);
        if (!TierBox)
        {
            UE_LOG(LogTemp, Error, TEXT("CreateDefaultTierBoxes: Failed to create TierBox %d"), i);
            continue;
        }
        
        // Calculate tier range
        float StartPercent = Tier.MinScore;
        float EndPercent = (i + 1 < Tiers.Num()) ? Tiers[i + 1].MinScore : 100.0f;
        
        FString TierName = UEnum::GetValueAsString(Tier.TierSelected);
        UE_LOG(LogTemp, Warning, TEXT("Tier %d: %s (%.1f%%-%.1f%%)"), 
            i, *TierName, StartPercent, EndPercent);
        
        // Initialize tier data
        TierBox->InitializeTier(Tier, EndPercent);
        
        // ✅ ADD TO CONTAINER FIRST (creates the slot!)
        TierBoxesContainer->AddChild(TierBox);
        TierBoxes.Add(TierBox);
        
        // ✅ NOW position it (slot exists now)
        float ScaledStart = (StartPercent / 100.0f) * MaxPosition;
        float ScaledEnd = (EndPercent / 100.0f) * MaxPosition;
        
        PositionTierBox(TierBox, ScaledStart, ScaledEnd);
        
        UE_LOG(LogTemp, Warning, TEXT("  Added and positioned, total boxes: %d"), TierBoxes.Num());
    }
    
    UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════"));
    UE_LOG(LogTemp, Warning, TEXT("Finished creating %d tier boxes"), TierBoxes.Num());
    UE_LOG(LogTemp, Warning, TEXT("═══════════════════════════════════════"));
}

void UHHoldReleaseMinigame::PositionTierBox(
    UHTierBox* TierBox,
    float StartPercent,
    float EndPercent)
{
    if (!TierBox)
    {
        UE_LOG(LogTemp, Error, TEXT("PositionTierBox: TierBox is NULL!"));
        return;
    }

    // Calculate position and size in pixels
    float NormalizedStart = StartPercent / MaxPosition;
    float NormalizedEnd = EndPercent / MaxPosition;
    
    float StartPixels = NormalizedStart * TrackWidth;
    float EndPixels = NormalizedEnd * TrackWidth;
    float WidthPixels = EndPixels - StartPixels;
    
    UE_LOG(LogTemp, Log, TEXT("PositionTierBox: Start=%.1f, End=%.1f, Width=%.1f pixels"),
        StartPixels, EndPixels, WidthPixels);
    
    // ✅ FIX: Use proper overlay slot configuration
    UOverlaySlot* HSlot = Cast<UOverlaySlot>(TierBox->Slot);
    
    if (HSlot)
    {
        // Set padding to position the box
        HSlot->SetPadding(FMargin(StartPixels, 0.0f, TrackWidth - EndPixels, 0.0f));
        HSlot->SetHorizontalAlignment(HAlign_Fill);
        HSlot->SetVerticalAlignment(VAlign_Fill);
        
        UE_LOG(LogTemp, Log, TEXT("  Applied padding: Left=%.1f, Right=%.1f"), 
            StartPixels, TrackWidth - EndPixels);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PositionTierBox: Slot is NULL!"));
    }
}

EMinigameTier UHHoldReleaseMinigame::GetTierAtPosition(float Position) const
{
    // Normalize position to 0-100% scale
    float NormalizedPos = (Position / MaxPosition) * 100.0f;
    
    for (const UHTierBox* TierBox : TierBoxes)
    {
        if (TierBox && TierBox->IsPositionInTier(NormalizedPos))
        {
            return TierBox->TierData.TierSelected;
        }
    }
    
    return EMinigameTier::Bad;
}

FLinearColor UHHoldReleaseMinigame::GetTierColor(
    EMinigameTier Tier,
    const TArray<FConsumableMinigameTier>& Tiers) const
{
    const FConsumableMinigameTier* TierData = Tiers.FindByPredicate(
        [Tier](const FConsumableMinigameTier& T) { return T.TierSelected == Tier; }
    );
    
    return TierData ? TierData->TierColor : FLinearColor::White;
}