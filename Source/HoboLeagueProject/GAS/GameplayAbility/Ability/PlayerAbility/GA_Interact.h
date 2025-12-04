// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInteractionComponent.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_Interact.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_Interact : public UHBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Interact();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	AHBaseItem* RetrieveInteractableItem(APlayerCharacter* Player, UHInteractionComponent* InteractionComp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact|Trace")
	bool bUseSphereTrace;

	/** 🔹 Max distance for both modes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact|Trace")
	float MaxDistance;

	/** 🔹 Sphere radius (only used in sphere mode) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact|Trace")
	float SphereRadius ;

	/** 🔹 Number of rays in line mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact|Trace")
	int32 NumRays;

	/** 🔹 Angle between each ray in degrees (line mode) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact|Trace")
	float AngleStep;

	/** 🔹 How long to show debug lines/spheres */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float DebugDrawTime;
};
