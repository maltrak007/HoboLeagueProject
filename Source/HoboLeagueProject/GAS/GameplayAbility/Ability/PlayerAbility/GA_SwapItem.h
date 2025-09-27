// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_SwapItem.generated.h"

enum class EItemType : uint8;
class UHInventoryComponent;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_SwapItem : public UHBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_SwapItem();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Swap")
	EItemType ItemTypeToSwap;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* SwapItemMontage;
protected:
	
private:
};
