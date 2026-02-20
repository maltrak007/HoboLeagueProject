// Fill out your copyright notice in the Description page of Project Settings.


#include "HConsumable.h"
#include "HConsumableDataAsset.h"
#include "HConsumableStatTable.h"

AHConsumable::AHConsumable()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AHConsumable::GetConsumableStatsValue(FConsumableStatsRow& OutStats) const
{
	const FConsumableStatsRow* Stats = GetConsumableStats();
	if (Stats)
	{
		OutStats = *Stats;
		return true;
	}
	return false;
}

void AHConsumable::BeginPlay()
{
	Super::BeginPlay();
	
	ConsumableData = Cast<UHConsumableDataAsset>(ItemData);
	
}

void AHConsumable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

const FConsumableStatsRow* AHConsumable::GetConsumableStats() const
{
	if (!ItemData || !ItemData->StatsTable)
	{
		return nullptr;
	}

	// Construct row name: "{ItemID}_{Rarity}"
	// e.g. "Syringe_Rare"
	FString RarityStr = UEnum::GetValueAsString(ItemRarity);
	RarityStr.RemoveFromStart("ERarityType::");
    
	const FName RowName = FName(*FString::Printf(TEXT("%s_%s"),
		*ItemData->ItemID.ToString(),
		*RarityStr));

	// Lookup in consumable stats table
	const FConsumableStatsRow* Row = ItemData->StatsTable->FindRow<FConsumableStatsRow>(
		RowName,
		TEXT("GetConsumableStats")
	);

	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("No consumable stats for '%s' at rarity '%s'"),
			*ItemData->ItemID.ToString(), *RarityStr);
	}

	return Row;
}

