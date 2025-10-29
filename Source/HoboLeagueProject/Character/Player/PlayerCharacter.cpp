// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "BasePlayerCharacterState.h"
#include "HoboLeagueProject/Character/BaseCharacterController.h"
#include "HoboLeagueProject/Component/HInteractionComponent.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/Component/HOverdriveComponent.h"
#include "HoboLeagueProject/Component/HStatusHandlerComponent.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/HAttributeSet.h"
#include "HoboLeagueProject/UI/HoboHUD.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetNetUpdateFrequency(100.f);
	InventoryComponent = CreateDefaultSubobject<UHInventoryComponent>(TEXT("InventoryComponent"));
	StatusHandlerComponent = CreateDefaultSubobject<UHStatusHandlerComponent>(TEXT("StatusHandlerComponent"));
	InteractionComponent = CreateDefaultSubobject<UHInteractionComponent>(TEXT("InteractionComponent"));
	OverdriveComponent = CreateDefaultSubobject<UHOverdriveComponent>(TEXT("OverdriveComponent"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

//Server side init
void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	
	if (InventoryComponent)
	{
		InventoryComponent->LinkAbilitySystemComponent(HAbilitySystemComponent); 
	}

	if (StatusHandlerComponent)
	{
		StatusHandlerComponent->LinkAbilitySystemComponent(HAbilitySystemComponent); 
	}

	if(OverdriveComponent)
	{
		OverdriveComponent->LinkASC(HAbilitySystemComponent);
	}
	
	HAbilitySystemComponent->ApplyInitialEffects();
	HAbilitySystemComponent->GiveInitialAbilities();

	InitActorHUD();
}

//Client side init
void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();

	if (InventoryComponent)
	{
		InventoryComponent->LinkAbilitySystemComponent(HAbilitySystemComponent); 
	}
	
	if (StatusHandlerComponent)
	{
		StatusHandlerComponent->LinkAbilitySystemComponent(HAbilitySystemComponent); 
	}

	if(OverdriveComponent)
	{
		OverdriveComponent->LinkASC(HAbilitySystemComponent);
	}
	
	HAbilitySystemComponent->ApplyInitialEffects();
	HAbilitySystemComponent->GiveInitialAbilities();

	InitActorHUD();
}

void APlayerCharacter::InitAbilityActorInfo()
{
	// Server init ability actor
	ABasePlayerCharacterState* HoboPlayerState = Cast<ABasePlayerCharacterState>(GetPlayerState());
	check(HoboPlayerState);
	HoboPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(HoboPlayerState, this);
	//Este da error si no lo casteo por motivos de conversion al tener que castear un *AbilitySystem y no el *AbilitySystem custom que creamos
	HAbilitySystemComponent = Cast<UHAbilitySystemComponent>(HoboPlayerState->GetAbilitySystemComponent()); 
	HAttributeSet = HoboPlayerState->GetAttributeSet();
}

void APlayerCharacter::InitActorHUD()
{
	if (ABaseCharacterController* HoboPlayerController = Cast<ABaseCharacterController>(GetController()))
	{
		if (AHoboHUD* HoboHUD = Cast<AHoboHUD>(HoboPlayerController->GetHUD()))
		{
			if (ABasePlayerCharacterState* HoboPlayerState = Cast<ABasePlayerCharacterState>(GetPlayerState()))
			{
				HoboHUD->InitOverlay(HoboPlayerController,HoboPlayerState,HAbilitySystemComponent,HAttributeSet);
			}
		}
	}
}


