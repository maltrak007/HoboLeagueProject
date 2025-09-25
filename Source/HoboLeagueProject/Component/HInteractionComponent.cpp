// Fill out your copyright notice in the Description page of Project Settings.


#include "HInteractionComponent.h"

#include "HoboLeagueProject/Item/HBaseItem.h"


UHInteractionComponent::UHInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ClearInteractableObjects();
}

void UHInteractionComponent::AddInteractableObject(AHBaseItem* InteractableObject)
{
	if (!InteractableObject) return;
	
	if (!NearbyInteractableObjects.Contains(InteractableObject))
	{
		NearbyInteractableObjects.Add(InteractableObject);
	}
}

void UHInteractionComponent::RemoveInteractableObject(AHBaseItem* InteractableObject)
{
	if (!InteractableObject) return;

	for (int32 i = NearbyInteractableObjects.Num() - 1; i >= 0; --i)
	{
		if (NearbyInteractableObjects[i] == InteractableObject)
		{
			NearbyInteractableObjects.RemoveAt(i);
		}
	}
}
