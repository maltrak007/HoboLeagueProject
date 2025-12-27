// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "HoboWidgetController.h"
#include "OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedSignature, float, NewStamina);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxStaminaChangedSignature, float, NewMaxStamina);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverdoseChangedSignature, float, NewOverdose);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxOverdoseChangedSignature, float, NewMaxOverdose);

//** Finish the rest of the class to display the changes in the HUD
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargedBarSignature, float, NewChargedBar);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxChargedBarSignature, float, NewMaxChargedBar);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class HOBOLEAGUEPROJECT_API UOverlayWidgetController : public UHoboWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	void BindCallbacksToAttributes();

	void HealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
	void MaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;

	void StaminaChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
	void MaxStaminaChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;

	void OverdoseChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
	void MaxOverdoseChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;

	UPROPERTY(BlueprintAssignable, Category="GAS||Attributes")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS||Attributes")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS||Attributes")
	FOnStaminaChangedSignature OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS||Attributes")
	FOnMaxStaminaChangedSignature OnMaxStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS||Attributes")
	FOnOverdoseChangedSignature OnOverdoseChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS||Attributes")
	FOnMaxOverdoseChangedSignature OnMaxOverdoseChanged;
};
