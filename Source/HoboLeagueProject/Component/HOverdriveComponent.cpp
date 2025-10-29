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

	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Overdrive.Consumable")),
	                                                 EGameplayTagEventType::AnyCountChange).AddUObject(
		this, &UHOverdriveComponent::OnItemConsumableTagChanged);
}

void UHOverdriveComponent::CheckOverdriveCombination()
{
	if (!AbilitySystemComponent) return;

	FGameplayTagContainer TagContainer = AbilitySystemComponent->GetOwnedGameplayTags();

	for (auto Element : OverdriveCombinationDataAsset->GetDataCombinations())
	{
		FGameplayTagContainer OverdriveRequiredTagContainer = Element.GetRequiredTags();
		if (TagContainer.HasAll(OverdriveRequiredTagContainer) && OverdriveRequiredTagContainer.Num() >= 2)
		{
			for (auto GameplayEffectClass : Element.GetGameplayEffects())
			{
				AbilitySystemComponent->ApplyGameplayEffectToSelf(GameplayEffectClass.GetDefaultObject(), 1.0f,
				                                                  AbilitySystemComponent->MakeEffectContext());
			}
		}
	}
}

void UHOverdriveComponent::OnItemConsumableTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if(NewCount > 0)
	{
		CheckOverdriveCombination();
	}
}
