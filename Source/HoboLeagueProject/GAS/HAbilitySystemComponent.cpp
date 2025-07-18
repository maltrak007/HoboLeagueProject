// Fill out your copyright notice in the Description page of Project Settings.


#include "HAbilitySystemComponent.h"


UHAbilitySystemComponent::UHAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHAbilitySystemComponent::ApplyInitialEffects()
{
	if(!GetOwner() || !GetOwner()->HasAuthority()) return; 
	for (const TSubclassOf<UGameplayEffect>& Effect : InitialEffects)
	{
		if (Effect)
		{
			const UGameplayEffect* GE = Effect->GetDefaultObject<UGameplayEffect>();
			if (GE)
			{
				ApplyGameplayEffectToSelf(GE, 1.0f, MakeEffectContext());
			}
		}
	}
	if(StartUpAbilitiesGiven)
	{
		for (const TSubclassOf<UGameplayAbility>& Ability : Abilities)
		{
			if (Ability)
			{
				GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
			}
		}
		StartUpAbilitiesGiven = true;
	}

}

void UHAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

