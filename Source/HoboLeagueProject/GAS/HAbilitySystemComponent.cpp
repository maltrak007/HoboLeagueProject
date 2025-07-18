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
			ApplyGameplayEffectToSelf(Effect.GetDefaultObject(), 1.0f, MakeEffectContext());
		}
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

