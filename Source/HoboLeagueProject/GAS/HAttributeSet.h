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
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) \

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
private:
	UPROPERTY()
	FGameplayAttributeData Health;
	
	UPROPERTY()
	FGameplayAttributeData MaxHealth;
	
	UPROPERTY()
	FGameplayAttributeData Stamina;
	
	UPROPERTY()
	FGameplayAttributeData MaxStamina;
};
