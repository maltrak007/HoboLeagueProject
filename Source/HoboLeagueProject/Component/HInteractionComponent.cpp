// Fill out your copyright notice in the Description page of Project Settings.


#include "HInteractionComponent.h"

#include "GameFramework/PlayerState.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/Item/HBaseItem.h"


UHInteractionComponent::UHInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UHInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ClearInteractableObjects();
}

void UHInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (const TScriptInterface<IItemInteractableInterface>& InteractableObject : NearbyInteractableObjects)
	{
		if (InteractableObject.GetObject())
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				0.f, // duration 0 means refresh every frame
				FColor::Green,
				FString::Printf(TEXT("Nearby Interactable Object: %s"), *InteractableObject.GetObject()->GetName())
			);
		}
	}
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
