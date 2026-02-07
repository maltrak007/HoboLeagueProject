// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HoboLeagueGamemode.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API AHoboLeagueGamemode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AHoboLeagueGamemode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	void RestartPlayer(AController* NewPlayer);
};
