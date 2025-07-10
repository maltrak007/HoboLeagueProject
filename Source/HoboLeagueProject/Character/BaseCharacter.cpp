// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/HAttributeSet.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// GAS Implementation
UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return HAbilitySystemComponent;
}

