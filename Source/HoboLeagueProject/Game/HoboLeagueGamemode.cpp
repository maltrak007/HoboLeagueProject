// Fill out your copyright notice in the Description page of Project Settings.


#include "HoboLeagueGamemode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/Item/PlayerItem/Weapon/HWeapon.h"


AHoboLeagueGamemode::AHoboLeagueGamemode()
{
	bUseSeamlessTravel = true;
}

void AHoboLeagueGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game %d"), NumberOfPlayers)
			);

			APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
			if (PlayerState)
			{
				FString PlayerName = PlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(
					-1,
					10.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game"), *PlayerName)
				);
			}
		}
		UWorld* World = GetWorld();
		FName CurrentMapName = *World->GetMapName(); // Note: May include prefix like "UEDPIE_0_"
		CurrentMapName = FPackageName::GetShortFName(CurrentMapName); // Remove PIE or persistent level prefix

		if (NumberOfPlayers == 3 && CurrentMapName == "MultiplayerLobbyGym")
		{
			UWorld* World2 = GameState->GetWorld();
			if (World2)
			{
				bUseSeamlessTravel = true;
				World2->ServerTravel(FString("/Game/Prototype/Gyms/CoreMechanicsGym?listen"));
			}
		}
	}
}

void AHoboLeagueGamemode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(
			1,
			60.f,
			FColor::Yellow,
			FString::Printf(TEXT("Players in game %d"), NumberOfPlayers - 1)
		);

		FString PlayerName = PlayerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s has exited the game"), *PlayerName)
		);
	}
}

// Override RestartPlayer to ensure players receive their initial inventory items upon respawn
void AHoboLeagueGamemode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	if (!NewPlayer) return;
	
	APawn* PlayerPawn = NewPlayer->GetPawn();
	if (PlayerPawn)
	{
		// Spawn initial inventory items for the player
		UHInventoryComponent* Inv = PlayerPawn->FindComponentByClass<UHInventoryComponent>();
		if (Inv)
		{
			const TArray<TSubclassOf<AHPlayerItem>>& ItemsToSpawn = Inv->GetInitialInventoryItems();

			for (const TSubclassOf<AHPlayerItem>& ItemClass : ItemsToSpawn)
			{
				if (ItemClass)
				{
					FActorSpawnParameters Params;
					Params.Owner = PlayerPawn;
					Params.Instigator = PlayerPawn;

					AHPlayerItem* NewItem = GetWorld()->SpawnActor<AHPlayerItem>(ItemClass, Params);
					if (NewItem)
					{
						Inv->AddItem(NewItem);
					}
				}
			}
		}
	}
}