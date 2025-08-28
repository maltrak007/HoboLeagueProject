// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HInventoryComponent.generated.h"


enum class EItemType : uint8;
class AHBaseItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	

};
