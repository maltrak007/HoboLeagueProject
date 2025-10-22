// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HoboLeagueProject/Item/PlayerItem/Consumable/HOverdriveCombinationDataAsset.h"
#include "HOverdriveComponent.generated.h"


class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHOverdriveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHOverdriveComponent();

	void LinkASC(UAbilitySystemComponent* _ASC);
	
	void CheckOverdriveCombination(FGameplayTagContainer CurrentTags);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Overdrive")
	TObjectPtr<UHOverdriveCombinationDataAsset> OverdriveCombinationDataAsset;

	UAbilitySystemComponent* AbilitySystemComponent;
};
