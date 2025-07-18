// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UHAbilitySystemComponent();
	
	void ApplyInitialEffects();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category="Initial Effects", meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditAnywhere, Category="Initial Effects", meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY()
	bool StartUpAbilitiesGiven = false;
};
