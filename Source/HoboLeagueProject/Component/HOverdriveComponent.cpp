// Fill out your copyright notice in the Description page of Project Settings.


#include "HOverdriveComponent.h"
#include "AbilitySystemComponent.h"

// Sets default values for this component's properties
UHOverdriveComponent::UHOverdriveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHOverdriveComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHOverdriveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Overdrive.Consumable")),
	                                                 EGameplayTagEventType::AnyCountChange).RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

void UHOverdriveComponent::LinkASC(UAbilitySystemComponent* _ASC)
{
	AbilitySystemComponent = _ASC;

	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Overdrive.Consumable")),
	                                                 EGameplayTagEventType::AnyCountChange).AddUObject(
		this, &UHOverdriveComponent::OnItemConsumableTagChanged);
}

void UHOverdriveComponent::CheckOverdriveCombination() const
{
	if (!AbilitySystemComponent || !OverdriveCombinationDataAsset) return;

	const FGameplayTagContainer& TagContainer =
		AbilitySystemComponent->GetOwnedGameplayTags();

	const auto& Combinations =
		OverdriveCombinationDataAsset->GetDataCombinations();

	FGameplayEffectContextHandle Context =
		AbilitySystemComponent->MakeEffectContext();

	for (const auto& Element : Combinations)
	{
		const FGameplayTagContainer& RequiredTags =
			Element.GetRequiredTags();

		if (RequiredTags.Num() < 2) continue;

		if (!TagContainer.HasAll(RequiredTags)) continue;

		for (const auto& EffectClass : Element.GetGameplayEffects())
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(
				EffectClass.GetDefaultObject(),
				1.f,
				Context);
		}
	}
}

void UHOverdriveComponent::OnItemConsumableTagChanged(FGameplayTag Tag, int32 NewCount) const
{
	if (NewCount > 0)
	{
		CheckOverdriveCombination();
	}
}
