// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "BasePlayerCharacterState.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/Component/HStatusHandlerComponent.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	
	InventoryComponent = CreateDefaultSubobject<UHInventoryComponent>(TEXT("InventoryComponent"));
	StatusHandlerComponent = CreateDefaultSubobject<UHStatusHandlerComponent>(TEXT("StatusHandlerComponent"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Server side init
void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	
	if (InventoryComponent)
	{
		InventoryComponent->LinkAbilitySystemComponent(); 
	}

	if (StatusHandlerComponent)
	{
		StatusHandlerComponent->LinkAbilitySystemComponent(); 
	}
	
	HAbilitySystemComponent->ApplyInitialEffects();
	HAbilitySystemComponent->GiveInitialAbilities();
}

//Client side init
void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();

	if (InventoryComponent)
	{
		InventoryComponent->LinkAbilitySystemComponent(); 
	}
	
	if (StatusHandlerComponent)
	{
		StatusHandlerComponent->LinkAbilitySystemComponent(); 
	}
	
	HAbilitySystemComponent->ApplyInitialEffects();
	HAbilitySystemComponent->GiveInitialAbilities();
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



