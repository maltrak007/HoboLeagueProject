// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void ApplyInitialEffects();

protected:


private:
	UPROPERTY(EditAnywhere, Category="Initial Effects", meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
};
