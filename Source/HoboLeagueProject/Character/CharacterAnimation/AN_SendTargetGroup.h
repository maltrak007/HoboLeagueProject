// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "AN_SendTargetGroup.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UAN_SendTargetGroup : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Hobo | Gameplay Ability")
	FGameplayTag EventTag;
	
	UPROPERTY(EditAnywhere, Category = "Hobo | Gameplay Ability")
	TArray<FName> TargetSocketNames;
};
