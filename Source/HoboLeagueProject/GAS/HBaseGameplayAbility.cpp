// Fill out your copyright notice in the Description page of Project Settings.


#include "HBaseGameplayAbility.h"

#include "AbilitySystemComponent.h"

void UHBaseGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if(IsPassiveAbility)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
