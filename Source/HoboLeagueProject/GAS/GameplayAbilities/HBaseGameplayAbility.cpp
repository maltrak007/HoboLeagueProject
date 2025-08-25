// Fill out your copyright notice in the Description page of Project Settings.


#include "HBaseGameplayAbility.h"

#include "AbilitySystemComponent.h"

UAnimInstance* UHBaseGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMesh = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMesh)
	{
		return OwnerSkeletalMesh->GetAnimInstance();
	}
	return nullptr;
}

void UHBaseGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (IsPassiveAbility)
	{
		//TODO MAybe this should be called by event
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
