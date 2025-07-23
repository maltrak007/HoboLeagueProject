// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HBaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UAnimInstance* GetOwnerAnimInstance() const;
};
