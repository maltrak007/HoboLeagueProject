// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HStatusHandlerComponent.generated.h"

struct FGameplayTag;
class UHAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHStatusHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHStatusHandlerComponent();

	/** Link the Ability System Component from the player state **/
	void LinkAbilitySystemComponent();
	
protected:
	
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ---------- Death / Respawn ---------- //
	UFUNCTION()
	void OnDeathTagChanged(FGameplayTag Tag, int32 NewCount);
	
	UFUNCTION()
	void HandlePlayerDeath() const;

	UFUNCTION()
	void HandlePlayerRespawn() const;
	
	// ---------- Overdose ---------- //
	UFUNCTION()
	void OnOverdoseTagChanged(FGameplayTag Tag, int32 NewCount);
	
	UFUNCTION()
	void HandlePlayerOverdose();
	// ----------------------------------------- //
	
	UPROPERTY()
	UHAbilitySystemComponent* ASC = nullptr;
};
