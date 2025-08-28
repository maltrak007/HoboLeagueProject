// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "AbilitySystemComponent.h"

#include "BasePlayerCharacterState.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/Item/HBaseItem.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
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

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//Server side init
	InitAbilityActorInfo();
	HAbilitySystemComponent->ApplyInitialEffects();
	HAbilitySystemComponent->GiveInitialAbilities();
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//Client side init
	InitAbilityActorInfo();
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

void APlayerCharacter::EquipItem(AHBaseItem* Item)
{
	if (!Item || !HAbilitySystemComponent || !Item->ItemData) return;
	
	const UHBaseItemDataAsset* Data = Item->ItemData;

	if (Data->GetItemPrimaryAbility())
	{
		HAbilitySystemComponent->GrantAndBindItemAbilityToInputID(EHAbilityInputID::BasicAttack, Data->GetItemPrimaryAbility());
	}

	if (Data->GetItemSecondaryAbility())
	{
		HAbilitySystemComponent->GrantAndBindItemAbilityToInputID(EHAbilityInputID::SecondaryAttack, Data->GetItemSecondaryAbility());
	}
}


void APlayerCharacter::Server_EquipItem_Implementation(AHBaseItem* Item)	
{
	if (!Item || !HasAuthority()) return;

	EquipItem(Item);
}


