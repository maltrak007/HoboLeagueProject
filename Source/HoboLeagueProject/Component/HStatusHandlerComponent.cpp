// Fill out your copyright notice in the Description page of Project Settings.


#include "HStatusHandlerComponent.h"
#include "GameFramework/PlayerState.h"
#include "HoboLeagueProject/Character/Player/BasePlayerCharacterState.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/GameplayAbility/Ability/PlayerAbility/GA_RegenerateStamina.h"


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
}

void UHStatusHandlerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_Dead)),
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

		ASC->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_Overdosing)),
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UHStatusHandlerComponent::LinkAbilitySystemComponent()
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		if (APlayerState* PS = PawnOwner->GetPlayerState())
		{
			ASC = PS->FindComponentByClass<UHAbilitySystemComponent>();

			//I Choose this approach because this method automatically replicates the changes to all clients
			// and it is more efficient than using a multicast RPC
			if (ASC)
			{
				ASC->RegisterGameplayTagEvent(
					FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_Dead)),
					EGameplayTagEventType::NewOrRemoved).AddUObject(
					this, &UHStatusHandlerComponent::OnDeathTagChanged);
				

				ASC->RegisterGameplayTagEvent(
					FGameplayTag::RequestGameplayTag(
						FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_Overdosing)),
					EGameplayTagEventType::NewOrRemoved).AddUObject(
					this, &UHStatusHandlerComponent::OnOverdoseTagChanged);
			}
		}
	}
}

// ---------- Death / Respawn ---------- //
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

// ---------- Overdose ---------- //
void UHStatusHandlerComponent::OnOverdoseTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		HandlePlayerOverdose();
	}
}

void UHStatusHandlerComponent::HandlePlayerOverdose()
{
	// START A TIMER TO KILL THE PLAYER AFTER A FEW SECONDS
	// SHOW WIDGET TO INDICATE THE TIME LEFT TO DIE
	// IF THE PLAYER USES AN ANTIDOTE, CLEAR THE TIMER AND REMOVE THE WIDGET
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PLAYER IS OVERDOSING"));
}

// ----------------------------------------- //
