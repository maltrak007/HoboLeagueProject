// Fill out your copyright notice in the Description page of Project Settings.

#include "HAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSet, MaxMovementSpeed, COND_None, REPNOTIFY_Always);
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
	if (Attribute == GetMovementSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMovementSpeed());
		SyncMovementSpeed(NewValue);
	}
}

void UHAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMovementSpeedAttribute())
	{
		SyncMovementSpeed(NewValue);
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
	if (Data.EvaluatedData.Attribute == GetMovementSpeedAttribute())
	{
		SetMovementSpeed(FMath::Clamp(GetMovementSpeed(), 0.f, GetMaxMovementSpeed()));
		SyncMovementSpeed(GetMovementSpeed());
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

void UHAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, MovementSpeed, OldMovementSpeed);
	SyncMovementSpeed(GetMovementSpeed());
}

void UHAttributeSet::OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSet, MaxMovementSpeed, OldMaxMovementSpeed);
}

void UHAttributeSet::SyncMovementSpeed(const float NewSpeed) const
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC) return;
	
	AActor* Avatar = ASC->GetAvatarActor();
	
	if (ACharacter* OwningCharacter = Cast<ACharacter>(Avatar))
	{
		if (UCharacterMovementComponent* MoveComp = OwningCharacter->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = NewSpeed;
		}
	}
}
