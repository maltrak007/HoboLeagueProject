// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/Item/NonPlayerItem/HNonPlayerItem.h"
#include "ItemGenerator.generated.h"

UCLASS()
class HOBOLEAGUEPROJECT_API AItemGenerator : public AHNonPlayerItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
