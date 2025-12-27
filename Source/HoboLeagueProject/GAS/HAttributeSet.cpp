// Fill out your copyright notice in the Description page of Project Settings.

#include "HAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

void UHAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, Overdose, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, MaxOverdose, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, ChargeBar, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, MaxChargeBar, COND_None, REPNOTIFY_Always);
}

void UHAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	if (Attribute == GetOverdoseAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxOverdose());
	}
	if(Attribute == GetChargeBarAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f, GetMaxChargeBar());
	}
}

void UHAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
	if (Data.EvaluatedData.Attribute == GetOverdoseAttribute())
	{
		SetOverdose(FMath::Clamp(GetOverdose(), 0.f, GetMaxOverdose()));
	}
	if (Data.EvaluatedData.Attribute == GetChargeBarAttribute())
	{
		SetChargeBar(FMath::Clamp(GetChargeBar(), 0.f, GetMaxChargeBar()));
	}
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
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, Overdose, OldOverdose);
}

void UHAttributeSet::OnRep_MaxOverdose(const FGameplayAttributeData& OldMaxOverdose) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, MaxOverdose, OldMaxOverdose);
}

void UHAttributeSet::OnRep_ChargeBar(const FGameplayAttributeData& OldChargeBar) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, ChargeBar, OldChargeBar);
}

void UHAttributeSet::OnRep_MaxChargeBar(const FGameplayAttributeData& OldMaxChargeBar) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, MaxChargeBar, OldMaxChargeBar);
}
