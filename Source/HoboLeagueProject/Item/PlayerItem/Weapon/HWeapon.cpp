#include "HWeapon.h"

#include "WeaponDataAsset.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"

AHWeapon::AHWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponData = Cast<UWeaponDataAsset>(ItemData);

	if (WeaponData)
	{
		RemainingDurability = WeaponData->GetTotalDurability();
	}
}

void AHWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHWeapon::ReduceWeaponDurability()
{
	if (!WeaponData) return;

	RemainingDurability = FMath::Clamp(
		RemainingDurability - WeaponData->GetLostDurability(),
		0.0f,
		WeaponData->GetTotalDurability()
	);
	
	// GEngine->AddOnScreenDebugMessage(
	// 	-1, 5.f, FColor::Red,
	// 	FString::Printf(TEXT("Durability Left: %f"), RemainingDurability)
	// );

	if (RemainingDurability <= 0)
	{
		if (UHInventoryComponent* InvComp = OwningPlayer->GetInventoryComponent())
		{
			RestoreWeaponProperties();
			InvComp->RemoveItem(this);
		}
	}
}

void AHWeapon::RestoreWeaponProperties()
{
	RemainingDurability = WeaponData->GetTotalDurability();
}
// PRAGMA_DISABLE_OPTIMIZATION
// PRAGMA_ENABLE_OPTIMIZATION
