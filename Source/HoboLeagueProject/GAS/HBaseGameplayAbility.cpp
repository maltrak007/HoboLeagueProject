// Fill out your copyright notice in the Description page of Project Settings.


#include "HBaseGameplayAbility.h"

UAnimInstance* UHBaseGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMesh = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMesh)
	{
		return OwnerSkeletalMesh->GetAnimInstance();
	}
	return nullptr;
}
