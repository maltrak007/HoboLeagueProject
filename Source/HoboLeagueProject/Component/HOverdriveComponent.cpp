// Fill out your copyright notice in the Description page of Project Settings.


#include "HOverdriveComponent.h"

#include "AbilitySystemComponent.h"


// Sets default values for this component's properties
UHOverdriveComponent::UHOverdriveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHOverdriveComponent::LinkASC(UAbilitySystemComponent* _ASC)
{
	AbilitySystemComponent = _ASC;
}

void UHOverdriveComponent::CheckOverdriveCombination(FGameplayTagContainer CurrentTags)
{
	FGameplayTagContainer TagContainer = AbilitySystemComponent->GetOwnedGameplayTags();
	if (TagContainer.HasAllExact(CurrentTags))
	{
		OverdriveCombinationDataAsset->GetDataCombinations();
	}
}







