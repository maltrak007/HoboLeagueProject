// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerCharacterState.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/HAttributeSet.h"

ABasePlayerCharacterState::ABasePlayerCharacterState()
{
	HAbilitySystemComponent = CreateDefaultSubobject<UHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	HAbilitySystemComponent->SetIsReplicated(true);
	HAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	HAttributeSet = CreateDefaultSubobject<UHAttributeSet>(TEXT("AttributeSet"));
	
	//Multiplayer Actualization Settings
	SetNetUpdateFrequency(100.0f);
}

UAbilitySystemComponent* ABasePlayerCharacterState::GetAbilitySystemComponent() const
{
	return HAbilitySystemComponent;
}

UHAttributeSet* ABasePlayerCharacterState::GetAttributeSet() const
{
	return HAttributeSet;
}
