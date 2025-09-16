// Fill out your copyright notice in the Description page of Project Settings.


#include "HStatusHandlerComponent.h"
#include "GameFramework/PlayerState.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"


// Sets default values for this component's properties
UHStatusHandlerComponent::UHStatusHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UHStatusHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	//Subscribe to the delegate that will be called when the player is dead
}

void UHStatusHandlerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
}

void UHStatusHandlerComponent::BindDeathDelegate()
{
	
}

void UHStatusHandlerComponent::HandlePlayerDeath()
{
	
}

void UHStatusHandlerComponent::LinkAbilitySystemComponent()
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		if (APlayerState* PS = PawnOwner->GetPlayerState())
		{
			ASC = PS->FindComponentByClass<UHAbilitySystemComponent>();
		}
	}
}


