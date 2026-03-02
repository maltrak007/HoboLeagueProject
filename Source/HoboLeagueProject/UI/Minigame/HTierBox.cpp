// Fill out your copyright notice in the Description page of Project Settings.


#include "HTierBox.h"

void UHTierBox::NativeConstruct()
{
	Super::NativeConstruct();
	if (!Background)
	{
		UE_LOG(LogTemp, Error, TEXT("HTierBox: Background widget is NULL! Did you bind it in UMG?"));
	}
}

void UHTierBox::InitializeTier(const FConsumableMinigameTier& InTierData, float EndScore)
{
	TierData = InTierData;
	TierEndScore = EndScore;
    
	if (Background)
	{
		Background->SetColorAndOpacity(TierData.TierColor);
        
		UE_LOG(LogTemp, Log, TEXT("HTierBox: Set color to (%.2f, %.2f, %.2f, %.2f)"),
			TierData.TierColor.R, TierData.TierColor.G, TierData.TierColor.B, TierData.TierColor.A);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HTierBox: Cannot set color - Background is NULL!"));
	}
}
