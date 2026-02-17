// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "HAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UHAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UHAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UHAttributeSet, Stamina)
	ATTRIBUTE_ACCESSORS(UHAttributeSet, MaxStamina)
	ATTRIBUTE_ACCESSORS(UHAttributeSet, Overdose)
	ATTRIBUTE_ACCESSORS(UHAttributeSet, MaxOverdose)
	ATTRIBUTE_ACCESSORS(UHAttributeSet, ChargeBar)
	ATTRIBUTE_ACCESSORS(UHAttributeSet, MaxChargeBar)
	ATTRIBUTE_ACCESSORS(UHAttributeSet, MovementSpeed)
	ATTRIBUTE_ACCESSORS(UHAttributeSet, MaxMovementSpeed)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 *	Called just before any modification happens to an attribute. This is lower level than PreAttributeModify/PostAttribute modify.
	 *	There is no additional context provided here since anything can trigger this. Executed effects, duration based effects, effects being removed, immunity being applied, stacking rules changing, etc.
	 *	This function is meant to enforce things like "Health = Clamp(Health, 0, MaxHealth)" and NOT things like "trigger this extra thing if damage is applied, etc".
	 *	
	 *	NewValue is a mutable reference so you are able to clamp the newly applied value as well.
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	/**
	*	Called just after a GameplayEffect is executed to modify the base value of an attribute. No more changes can be made.
	*	Note this is only called during an 'execute'. E.g., a modification to the 'base value' of an attribute. It is not called during an application of a GameplayEffect, such as a 5 second +10 movement speed buff.
	*/
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;

	UPROPERTY(ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;

	UPROPERTY(ReplicatedUsing = OnRep_Overdose)
	FGameplayAttributeData Overdose;

	UPROPERTY(ReplicatedUsing = OnRep_MaxOverdose)
	FGameplayAttributeData MaxOverdose;

	UPROPERTY(ReplicatedUsing = OnRep_ChargeBar)
	FGameplayAttributeData ChargeBar;

	UPROPERTY(ReplicatedUsing = OnRep_MaxChargeBar)
	FGameplayAttributeData MaxChargeBar;
	
	UPROPERTY(ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_MaxMovementSpeed)
	FGameplayAttributeData MaxMovementSpeed;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina) const;

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const;

	UFUNCTION()
	void OnRep_Overdose(const FGameplayAttributeData& OldOverdose) const;

	UFUNCTION()
	void OnRep_MaxOverdose(const FGameplayAttributeData& OldMaxOverdose) const;

	UFUNCTION()
	void OnRep_ChargeBar(const FGameplayAttributeData& OldChargeBar) const;

	UFUNCTION()
	void OnRep_MaxChargeBar(const FGameplayAttributeData& OldMaxChargeBar) const;

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const;

	UFUNCTION()
	void OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed) const;
	
	UFUNCTION()
	void SyncMovementSpeed(float NewSpeed) const;
};
