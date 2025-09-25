// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HInteractionComponent.generated.h"

class AHBaseItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHInteractionComponent();

	UPROPERTY(EditDefaultsOnly)
	TArray<AHBaseItem*> NearbyInteractableObjects;
	
	//** Methods to add, remove and clear interactable objects */
	UFUNCTION()
	void AddInteractableObject(AHBaseItem* InteractableObject);
	
	UFUNCTION()
	void RemoveInteractableObject(AHBaseItem* InteractableObject);
	
	UFUNCTION()
	void ClearInteractableObjects() { NearbyInteractableObjects.Empty(); }

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
