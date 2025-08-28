// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueProject/Item/HBaseItem.h"
#include "HConsumable.generated.h"

UCLASS()
class HOBOLEAGUEPROJECT_API AHConsumable : public AHBaseItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHConsumable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
};
