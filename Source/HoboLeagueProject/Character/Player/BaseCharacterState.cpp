// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterState.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/HAttributeSet.h"

ABaseCharacterState::ABaseCharacterState()
{
	HAbilitySystemComponent = CreateDefaultSubobject<UHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	HAbilitySystemComponent->SetIsReplicated(true);
	HAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	HAttributeSet = CreateDefaultSubobject<UHAttributeSet>(TEXT("AttributeSet"));
	
	//Multiplayer Actualization Settings
	SetNetUpdateFrequency(100.0f);
}

UAbilitySystemComponent* ABaseCharacterState::GetAbilitySystemComponent() const
{
	return HAbilitySystemComponent;
}
