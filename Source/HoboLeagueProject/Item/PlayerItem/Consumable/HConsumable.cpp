// Fill out your copyright notice in the Description page of Project Settings.


#include "HConsumable.h"

#include "ConsumableDataAsset.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"

AHConsumable::AHConsumable()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHConsumable::BeginPlay()
{
	Super::BeginPlay();
	
	ConsumableData = Cast<UConsumableDataAsset>(ItemData);
	
	if (ConsumableData)
	{
		RemainingCharges = ConsumableData->GetCharges();
	}
}

void AHConsumable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void AHConsumable::ReduceConsumableCharges()
{
	if (!ConsumableData) return;
	
	RemainingCharges = FMath::Clamp(
		RemainingCharges - 1,
		0,
		ConsumableData->GetCharges()
	);
	
	if (RemainingCharges <= 0)
	{
		if (UHInventoryComponent* InvComp = OwningPlayer->GetInventoryComponent())
		{
			RestoreConsumableProperties();
			InvComp->RemoveItem(this);
		}
	}
}

void AHConsumable::RestoreConsumableProperties()
{
	RemainingCharges = ConsumableData->GetCharges();
}

