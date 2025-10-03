// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlayWidgetController.h"

#include "HoboLeagueProject/GAS/HAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	if (UHAttributeSet* HAttributeSet = Cast<UHAttributeSet>(AttributeSet))
	{
		OnHealthChanged.Broadcast(HAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(HAttributeSet->GetMaxHealth());
		OnStaminaChanged.Broadcast(HAttributeSet->GetStamina());
		OnMaxStaminaChanged.Broadcast(HAttributeSet->GetMaxStamina());
		OnOverdoseChanged.Broadcast(HAttributeSet->GetOverdose());
		OnMaxOverdoseChanged.Broadcast(HAttributeSet->GetMaxOverdose());
	}
}

void UOverlayWidgetController::BindCallbacksToAttributes()
{
	const UHAttributeSet* HAttributeSet = Cast<UHAttributeSet>(AttributeSet);
	if (!HAttributeSet || !AbilitySystemComponent) return;
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAttributeSet->GetHealthAttribute()).AddUObject(
		this, &UOverlayWidgetController::HealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAttributeSet->GetMaxHealthAttribute()).AddUObject(
		this, &UOverlayWidgetController::MaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAttributeSet->GetStaminaAttribute()).AddUObject(
		this, &UOverlayWidgetController::StaminaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAttributeSet->GetMaxStaminaAttribute()).AddUObject(
		this, &UOverlayWidgetController::MaxStaminaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAttributeSet->GetOverdoseAttribute()).AddUObject(
		this, &UOverlayWidgetController::OverdoseChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAttributeSet->GetMaxOverdoseAttribute()).
	                        AddUObject(this, &UOverlayWidgetController::MaxOverdoseChanged);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnHealthChanged.Broadcast(OnAttributeChangeData.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnMaxHealthChanged.Broadcast(OnAttributeChangeData.NewValue);
}

void UOverlayWidgetController::StaminaChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnStaminaChanged.Broadcast(OnAttributeChangeData.NewValue);
}

void UOverlayWidgetController::MaxStaminaChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnMaxStaminaChanged.Broadcast(OnAttributeChangeData.NewValue);
}

void UOverlayWidgetController::OverdoseChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnOverdoseChanged.Broadcast(OnAttributeChangeData.NewValue);
}

void UOverlayWidgetController::MaxOverdoseChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnMaxOverdoseChanged.Broadcast(OnAttributeChangeData.NewValue);
}
