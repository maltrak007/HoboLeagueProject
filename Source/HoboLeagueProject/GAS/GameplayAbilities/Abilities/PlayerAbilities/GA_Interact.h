// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInteractionComponent.h"
#include "HoboLeagueProject/GAS/GameplayAbilities/HBaseGameplayAbility.h"
#include "GA_Interact.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_Interact : public UHBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Interact();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	AHBaseItem* RetrieveInteractableItem(APlayerCharacter* Player, UHInteractionComponent* InteractionComp);
};
