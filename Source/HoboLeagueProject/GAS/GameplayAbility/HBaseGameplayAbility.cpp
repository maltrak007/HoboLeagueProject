// Fill out your copyright notice in the Description page of Project Settings.


#include "HBaseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Kismet/KismetSystemLibrary.h"

UHBaseGameplayAbility::UHBaseGameplayAbility()
{
	// Set reasonable defaults
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	IsPassiveAbility = false;
}

int32 UHBaseGameplayAbility::GetCurrentAbilityLevel() const
{
	return GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
}

void UHBaseGameplayAbility::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	if (UAbilityTask* AbilityTask = Cast<UAbilityTask>(&Task))
	{
		AbilityActiveTasks.Remove(AbilityTask);
	}
	
	Super::OnGameplayTaskDeactivated(Task);
}

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
	TSet<AActor*> HitActors;
	
	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
	{
		//** Get the start and end location of the trace from the target data */
		FVector StartLocation = TargetData->GetOrigin().GetTranslation();
		FVector EndLocation = TargetData->GetEndPoint();

		//** Define object types to trace */
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		//** Ignore self by default but active in case of doing abilities that can inflict self damage like a grenade */
		TArray<AActor*> IgnoreActors;
		if (bIgnoreSelf)
		{
			IgnoreActors.Add(GetAvatarActorFromActorInfo());
		}

		EDrawDebugTrace::Type DrawDebugType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		TArray<FHitResult> Results;

		UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLocation, EndLocation, SphereSweepRadius,
		                                                 ObjectTypes, false, IgnoreActors, DrawDebugType, Results, false);

		//Loop through results and add to out results if not already present
		//This is to avoid multiple hits on same actor from different traces
		for (const FHitResult& Result : Results)
		{
			if (HitActors.Contains(Result.GetActor()))
			{
				continue;
			}
			HitActors.Add(Result.GetActor());
			OutResults.Add(Result);
		}
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

void UHBaseGameplayAbility::RegisterTask(UAbilityTask* Task)
{
	if (!Task) return;

	// Use the renamed array
	AbilityActiveTasks.AddUnique(Task);
}
