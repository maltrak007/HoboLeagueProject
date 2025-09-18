#include "HWeapon.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"

AHWeapon::AHWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AHWeapon::Interact(AActor* InteractingActor)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(InteractingActor))
	{
		Player->InventoryComponent->AddItem(this);
	}
}








