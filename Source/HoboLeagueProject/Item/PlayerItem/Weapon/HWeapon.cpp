#include "HWeapon.h"

#include "WeaponDataAsset.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/GAS/GameplayAbility/Ability/PlayerAbility/GA_Attack.h"

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

		if (UGA_Attack* WeaponPAttack = Cast<UGA_Attack>(WeaponData->GetItemPrimaryAbility()))
		{
			WeaponPAttack->OnWeaponHitDelegate.AddUObject(this, &AHWeapon::ReduceWeaponDurability);
		}
		if (UGA_Attack* WeaponSAttack = Cast<UGA_Attack>(WeaponData->GetItemSecondaryAbility()))
		{
			WeaponSAttack->OnWeaponHitDelegate.AddUObject(this, &AHWeapon::ReduceWeaponDurability);
		}
	}
	
}

void AHWeapon::ReduceWeaponDurability()
{
	if (RemainingDurability > 0)
	{
		RemainingDurability -= WeaponData->GetLostDurability();

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf(TEXT("Durability Left: %f"), RemainingDurability));
		
		if (RemainingDurability <= 0)
		{
			if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetOwner()))
			{
				PlayerChar->InventoryComponent->RemoveItem(this);
			}
		}
	}
}







