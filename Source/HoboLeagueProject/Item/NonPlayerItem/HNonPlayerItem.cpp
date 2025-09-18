// Fill out your copyright notice in the Description page of Project Settings.


#include "HNonPlayerItem.h"


// Sets default values
AHNonPlayerItem::AHNonPlayerItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHNonPlayerItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHNonPlayerItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

