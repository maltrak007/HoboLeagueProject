// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "AN_SendGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UAN_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Hobo | Gameplay Ability")
	FGameplayTag EventTag;

	virtual FString GetNotifyName_Implementation() const override;
};
