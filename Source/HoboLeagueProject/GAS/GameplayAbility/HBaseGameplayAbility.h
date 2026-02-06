// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HoboLeagueProject/GAS/HGameplayAbilityTypes.h"
#include "HBaseGameplayAbility.generated.h"

enum class EHAbilityInputID : uint8;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Hobo | Ability Info")
	EHAbilityInputID AbilityInputID = EHAbilityInputID::None;
	
protected:
	UAnimInstance* GetOwnerAnimInstance() const;
	
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                                           float SphereSweepRadius = 30.f, bool bDrawDebug = false,
	                                                           bool bIgnoreSelf = true) const;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hobo | Ability Info")
	bool IsPassiveAbility = false;

	
};
