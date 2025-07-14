// Fill out your copyright notice in the Description page of Project Settings.


#include "HoboLeagueAssetManager.h"

#include "GAS/FGameplayTags.h"

UHoboLeagueAssetManager& UHoboLeagueAssetManager::Get()
{
	TObjectPtr<UHoboLeagueAssetManager> TT_AssetManager = Cast<UHoboLeagueAssetManager>(GEngine->AssetManager);
	return *TT_AssetManager;
}

void UHoboLeagueAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FHGameplayTags::InitializeNativeGameplayTags();
}
