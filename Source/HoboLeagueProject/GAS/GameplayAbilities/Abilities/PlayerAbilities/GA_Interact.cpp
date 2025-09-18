// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Interact.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInteractionComponent.h"
#include "HoboLeagueProject/Item/HBaseItem.h"
#include "HoboLeagueProject/Item/ItemInteractableInterface.h"

UGA_Interact::UGA_Interact()
{
	
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerCharacter* PC = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!PC) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UHInteractionComponent* InteractionComp = PC->FindComponentByClass<UHInteractionComponent>();
	if (!InteractionComp)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AHBaseItem* Target = RetrieveInteractableItem(PC, InteractionComp);
	if (Target)
	{
		// Call Interact on the target (via interface)
		IItemInteractableInterface::Execute_Interact(Target, PC);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

AHBaseItem* UGA_Interact::RetrieveInteractableItem(APlayerCharacter* Player, UHInteractionComponent* InteractionComp)
{
	if (!Player || !InteractionComp) return nullptr;

	UWorld* World = Player->GetWorld();
	if (!World) return nullptr;

	FVector Start = Player->GetActorLocation();
	FVector Forward = Player->GetActorForwardVector();

	// Settings
	const int32 NumRays = 6;
	const float AngleStep = 10.0f; // degrees between rays
	const float MaxDistance = 300.0f;

	AHBaseItem* BestTarget = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (int32 i = -NumRays/2; i <= NumRays/2; ++i)
	{
		FRotator SpreadRot = Forward.Rotation();
		SpreadRot.Yaw += i * AngleStep;

		FVector End = Start + SpreadRot.Vector() * MaxDistance;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Player);

		if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			if (AHBaseItem* HitActor = Cast<AHBaseItem>(Hit.GetActor()))
			{
				// Must implement interface and be in the interaction array
				if (HitActor->GetClass()->ImplementsInterface(UItemInteractableInterface::StaticClass()) &&
					InteractionComp->NearbyInteractableObjects.Contains(HitActor))
				{
					float DistSq = FVector::DistSquared(Start, HitActor->GetActorLocation());
					if (DistSq < BestDistSq)
					{
						BestDistSq = DistSq;
						BestTarget = HitActor;
					}
				}
			}
		}

		// Debug draw
		DrawDebugLine(World, Start, End, FColor::Green, false, 1.0f, 0, 1.5f);
	}

	return BestTarget;
}