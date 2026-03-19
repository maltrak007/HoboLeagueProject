// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HItemInteractableInterface.generated.h"

class APlayerCharacter;
// This class does not need to be modified.
UINTERFACE()
class UHItemInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HOBOLEAGUEPROJECT_API IHItemInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Called when a character interacts with an actor that implements this interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(APlayerCharacter* PlayerOwner);

	// Implement a method to highlight the objects if a bool is true, and unhighlight if it's false
	// Implement a method to show UI elements related to the object if a bool is true, and hide them if it's false
};
