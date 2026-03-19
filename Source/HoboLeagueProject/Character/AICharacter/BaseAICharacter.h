// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/Character/BaseCharacter.h"
#include "BaseAICharacter.generated.h"

class UBlackboardData;
class UBehaviorTree;

UCLASS()
class HOBOLEAGUEPROJECT_API ABaseAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	
	ABaseAICharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;
	
protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Helper")
	TObjectPtr<USceneComponent> EyePoint;
	
private:

};
