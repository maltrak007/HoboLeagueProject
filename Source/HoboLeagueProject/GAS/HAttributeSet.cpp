// Fill out your copyright notice in the Description page of Project Settings.

#include "HAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UHAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
}

void UHAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, Health, OldHealth);
}

void UHAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, MaxHealth, OldMaxHealth);
}

void UHAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, Stamina, OldStamina);
}

void UHAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, MaxStamina, OldMaxStamina);
}

void UHAttributeSet::OnRep_Overdose(const FGameplayAttributeData& OldOverdose) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, MaxStamina, OldOverdose);
}

void UHAttributeSet::OnRep_MaxOverdose(const FGameplayAttributeData& OldMaxOverdose) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, MaxStamina, OldMaxOverdose);
}
