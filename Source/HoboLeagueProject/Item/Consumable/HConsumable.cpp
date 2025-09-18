// Fill out your copyright notice in the Description page of Project Settings.


#include "HConsumable.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"


// Sets default values
AHConsumable::AHConsumable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHConsumable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHConsumable::Interact(AActor* InteractingActor)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(InteractingActor))
	{
		Player->InventoryComponent->AddItem(this);
	}
}


