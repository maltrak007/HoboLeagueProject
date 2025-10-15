#include "HWeapon.h"

#include "HWeaponDataAsset.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"

AHWeapon::AHWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponData = Cast<UHWeaponDataAsset>(ItemData);

	if (WeaponData)
	{
		RemainingDurability = WeaponData->GetTotalDurability();
	}
}

void AHWeapon::ReduceWeaponDurability()
{
	if (!WeaponData) return;

	RemainingDurability = FMath::Clamp(
		RemainingDurability - WeaponData->GetLostDurability(),
		0.0f,
		WeaponData->GetTotalDurability()
	);

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
