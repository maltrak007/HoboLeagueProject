#include "HWeapon.h"
#include "HWeaponDataAsset.h"
#include "HWeaponStatsTable.h"

AHWeapon::AHWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

const FWeaponStatsRow* AHWeapon::GetWeaponStats() const
{
	if (!ItemData || !ItemData->StatsTable)
	{
		return nullptr;
	}

	// Construct row name: "{ItemID}_{Rarity}"
	FString RarityStr = UEnum::GetValueAsString(ItemRarity);
	RarityStr.RemoveFromStart("ERarityType::");
    
	const FName RowName = FName(*FString::Printf(TEXT("%s_%s"),
		*ItemData->ItemID.ToString(),
		*RarityStr));

	// Lookup in weapon stats table
	const FWeaponStatsRow* Row = ItemData->StatsTable->FindRow<FWeaponStatsRow>(
		RowName,
		TEXT("GetWeaponStats")
	);

	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon stats for '%s' at rarity '%s'"),
			*ItemData->ItemID.ToString(), *RarityStr);
	}

	return Row;
}

void AHWeapon::BeginPlay()
{
	Super::BeginPlay();
}
