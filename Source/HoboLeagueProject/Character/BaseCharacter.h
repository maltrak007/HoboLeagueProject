// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UHAbilitySystemComponent;
class UHAttributeSet;

UCLASS(Abstract)
class HOBOLEAGUEPROJECT_API ABaseCharacter : public ACharacter , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UHAttributeSet* GetAttributeSet() const;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UHAbilitySystemComponent> HAbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UHAttributeSet> HAttributeSet;
};
