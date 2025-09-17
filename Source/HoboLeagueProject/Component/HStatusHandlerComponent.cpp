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
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Status.Dead")),
		                              EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UHStatusHandlerComponent::OnDeathTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		HandlePlayerDeath();
	}
	else
	{
		HandlePlayerRespawn();
	}
}

void UHStatusHandlerComponent::HandlePlayerDeath() const
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
		PawnOwner->DisableInput(PC);
		PawnOwner->SetActorEnableCollision(false);
		PawnOwner->SetActorHiddenInGame(true);
	}
}

void UHStatusHandlerComponent::HandlePlayerRespawn() const
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
		PawnOwner->EnableInput(PC);
		PawnOwner->SetActorEnableCollision(true);
		PawnOwner->SetActorHiddenInGame(false);
	}
}

void UHStatusHandlerComponent::LinkAbilitySystemComponent()
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		if (APlayerState* PS = PawnOwner->GetPlayerState())
		{
			ASC = PS->FindComponentByClass<UHAbilitySystemComponent>();
			
			if (ASC)
			{
				ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Status.Dead")),
											  EGameplayTagEventType::NewOrRemoved).AddUObject(
					this, &UHStatusHandlerComponent::OnDeathTagChanged);
			}
			
			// If needed, bind to player death event here
			// ASC->OnPlayerDeath.RemoveAll(this);
			// ASC->OnPlayerDeath.AddDynamic(this, &UHStatusHandlerComponent::HandlePlayerDeath);
		}
	}
}
