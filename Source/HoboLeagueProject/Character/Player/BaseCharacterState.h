// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "BaseCharacterState.generated.h"


class UHAbilitySystemComponent;
class UHAttributeSet;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API ABaseCharacterState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ABaseCharacterState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UHAttributeSet* GetAttributeSet() const;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UHAbilitySystemComponent> HAbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UHAttributeSet> HAttributeSet;
};
