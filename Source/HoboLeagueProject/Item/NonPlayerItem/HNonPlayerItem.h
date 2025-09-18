// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/Item/HBaseItem.h"
#include "HNonPlayerItem.generated.h"

UCLASS()
class HOBOLEAGUEPROJECT_API AHNonPlayerItem : public AHBaseItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHNonPlayerItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
