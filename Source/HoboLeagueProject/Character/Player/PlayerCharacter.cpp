// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "AbilitySystemComponent.h"

#include "BasePlayerCharacterState.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/Weapon/HoboLeagueWeapon.h"
#include "HoboLeagueProject/Weapon/WeaponDataAsset.h"
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

void APlayerCharacter::EquipWeapon(AHoboLeagueWeapon* Weapon)
{
	if (!Weapon || !HAbilitySystemComponent || !Weapon->WeaponData) return;
	
	const UWeaponDataAsset* Data = Weapon->WeaponData;

	if (Data->GetWeaponPrimaryAbility())
	{
		HAbilitySystemComponent->GrantWeaponAbility(EHAbilityInputID::BasicAttack, Data->GetWeaponPrimaryAbility());
	}

	if (Data->GetWeaponSecondaryAbility())
	{
		HAbilitySystemComponent->GrantWeaponAbility(EHAbilityInputID::SecondaryAttack, Data->GetWeaponSecondaryAbility());
	}
}


void APlayerCharacter::Server_EquipWeapon_Implementation(AHoboLeagueWeapon* Weapon)	
{
	if (!Weapon || !HasAuthority()) return;

	EquipWeapon(Weapon);
}


