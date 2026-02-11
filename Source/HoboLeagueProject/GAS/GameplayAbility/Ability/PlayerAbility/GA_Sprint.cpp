// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Sprint.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "AbilitySystemComponent.h"

UGA_Sprint::UGA_Sprint()
{
	// CRITICAL: Set these for proper multiplayer behavior
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	// Set ability activation properties
	bRetriggerInstancedAbility = false;
	
	// Configure tags (set these in Blueprint or here)
	// ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("Status.Sprinting"));
	// BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Action.Attack"));
	// CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Action.Reload"));
}

void UGA_Sprint::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// Commit the ability (checks cost, cooldown, etc.)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Cache character and movement component
	CachedCharacter = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!CachedCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedMoveComp = CachedCharacter->GetCharacterMovement();
	if (!CachedMoveComp)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Store original speed for restoration
	OriginalMaxWalkSpeed = CachedMoveComp->MaxWalkSpeed;

	// Apply sprint cost effect (stamina drain over time)
	if (SprintCostEffect)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
			SprintCostEffect,
			GetAbilityLevel()
		);
		
		if (SpecHandle.IsValid())
		{
			SprintEffectHandle = ApplyGameplayEffectSpecToOwner(
				Handle,
				ActorInfo,
				ActivationInfo,
				SpecHandle
			);
		}
	}

	// Apply speed change with proper authority/prediction
	// HasAuthorityOrPredictionKey ensures this runs on server and predicting client
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		float NewSpeed = SprintSpeed;
		
		// Use multiplier if configured
		if (bUseSpeedMultiplier)
		{
			NewSpeed = OriginalMaxWalkSpeed * SpeedMultiplier;
		}
		
		CachedMoveComp->MaxWalkSpeed = NewSpeed;
		
		UE_LOG(LogTemp, Log, TEXT("Sprint Activated - Speed: %.2f (Original: %.2f)"), 
			NewSpeed, OriginalMaxWalkSpeed);
	}
}

void UGA_Sprint::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// Remove sprint cost effect
	if (SprintEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(SprintEffectHandle);
		SprintEffectHandle.Invalidate();
	}

	// Restore original movement speed
	if (CachedMoveComp && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// Restore to original speed (or default if we never cached it)
		const float SpeedToRestore = (OriginalMaxWalkSpeed > 0.0f) ? OriginalMaxWalkSpeed : DefaultSpeed;
		CachedMoveComp->MaxWalkSpeed = SpeedToRestore;
		
		UE_LOG(LogTemp, Log, TEXT("Sprint Ended - Speed Restored: %.2f"), SpeedToRestore);
	}

	// Clear cached references
	CachedCharacter = nullptr;
	CachedMoveComp = nullptr;
	OriginalMaxWalkSpeed = 0.0f;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Sprint::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}