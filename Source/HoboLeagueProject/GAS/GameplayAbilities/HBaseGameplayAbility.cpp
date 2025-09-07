// Fill out your copyright notice in the Description page of Project Settings.


#include "HBaseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UAnimInstance* UHBaseGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMesh = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMesh)
	{
		return OwnerSkeletalMesh->GetAnimInstance();
	}
	return nullptr;
}

TArray<FHitResult> UHBaseGameplayAbility::GetHitResultFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius, bool bDrawDebug,
	bool bIgnoreSelf) const
{
	TArray<FHitResult> OutResults;
	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
	{
		FVector StartLocation = TargetData->GetOrigin().GetTranslation();
		FVector EndLocation = TargetData->GetEndPoint();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> IgnoreActors;
		if (bIgnoreSelf)
		{
			IgnoreActors.Add(GetAvatarActorFromActorInfo());
		}

		EDrawDebugTrace::Type DrawDebugType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		TArray<FHitResult> HitResults;

		UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLocation, EndLocation, SphereSweepRadius,
		                                                 ObjectTypes, false, IgnoreActors, DrawDebugType, HitResults, false);
	}
	return OutResults;
}

void UHBaseGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (IsPassiveAbility)
	{
		//TODO Maybe this should be called by event
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
