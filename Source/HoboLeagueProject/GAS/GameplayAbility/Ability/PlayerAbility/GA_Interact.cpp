// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Interact.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInteractionComponent.h"
#include "HoboLeagueProject/Item/HBaseItem.h"
#include "HoboLeagueProject/Item/HItemInteractableInterface.h"

static TAutoConsoleVariable<int32> CVarInteractDebugDraw(
	TEXT("ga.Interact.DebugDraw"),
	0,
	TEXT("Enable debug drawing for GA_Interact (0=off, 1=on)"),
	ECVF_Default
);

UGA_Interact::UGA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	APlayerCharacter* PC = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!PC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UHInteractionComponent* InteractionComp = PC->GetComponentByClass<UHInteractionComponent>();
	if (!InteractionComp)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AHBaseItem* Target = RetrieveInteractableItem(PC, InteractionComp);
	if (Target)
	{
		// ✅ ONLY SERVER executes the actual interaction
		if (HasAuthority(&ActivationInfo))
		{
			// Server: Authoritative execution
			IHItemInteractableInterface::Execute_Interact(Target, PC);
		}
		else
		{
			// ✅ Client: Predictive cosmetics (optional)
			// Play sounds, particles, etc. for immediate feedback
			// The actual interaction happens on server
            
			// Example predictive feedback:
			// PlayInteractSound();
			// SpawnInteractParticles();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

AHBaseItem* UGA_Interact::RetrieveInteractableItem(APlayerCharacter* Player, UHInteractionComponent* InteractionComp)
{
	const TArray<AHBaseItem*>& NearbyObjects = InteractionComp->NearbyInteractableObjects;
    
	if (NearbyObjects.Num() == 0)
	{
		return nullptr;
	}

	const FVector PlayerLocation = Player->GetActorLocation();
	const FVector PlayerForward = Player->GetActorForwardVector();
	const float MaxDistSq = MaxDistance * MaxDistance;

	AHBaseItem* BestTarget = nullptr;
	float BestScore = -1.0f;

	for (AHBaseItem* Item : NearbyObjects)
	{
		if (!IsValid(Item))
		{
			continue;
		}
		
		if (!Item->GetClass()->ImplementsInterface(UHItemInteractableInterface::StaticClass()))
		{
			continue;
		}

		const FVector ItemLocation = Item->GetActorLocation();
		const float DistSq = FVector::DistSquared(PlayerLocation, ItemLocation);

		if (DistSq > MaxDistSq)
		{
			continue;
		}

		const FVector ToItem = (ItemLocation - PlayerLocation).GetSafeNormal();
		const float DotProduct = FVector::DotProduct(PlayerForward, ToItem);

		if (DotProduct <= 0.0f)
		{
			continue;
		}

		const float Score = DotProduct / FMath::Max(DistSq, 1.0f);

		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Item;
		}
	}

	return BestTarget;
}
