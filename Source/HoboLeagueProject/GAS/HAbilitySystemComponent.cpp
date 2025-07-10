// Fill out your copyright notice in the Description page of Project Settings.


#include "HAbilitySystemComponent.h"


void UHAbilitySystemComponent::ApplyInitialEffects()
{
	for (const TSubclassOf<UGameplayEffect>& Effect : InitialEffects)
	{
		if (Effect)
		{
			ApplyGameplayEffectToSelf(Effect.GetDefaultObject(), 1.0f, MakeEffectContext());
		}
	}
}

