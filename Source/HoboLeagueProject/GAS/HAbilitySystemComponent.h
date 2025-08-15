// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HGameplayAbilityTypes.h"
#include "HAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UHAbilitySystemComponent();
	
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	void GrantItemAbility(EHAbilityInputID InputID, TSubclassOf<UGameplayAbility> AbilityClass);
	void UnBindAbility(EHAbilityInputID InputID, TSubclassOf<UGameplayAbility> AbilityClass);
private:
	UPROPERTY(EditAnywhere, Category="Hobo | Initial Effects", meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	UPROPERTY(EditAnywhere, Category="Hobo | Gameplay Abilities", meta=(AllowPrivateAccess="true"))
	TMap<EHAbilityInputID,TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditAnywhere, Category="Hobo | Gameplay BasicAbilities", meta=(AllowPrivateAccess="true"))
	TMap<EHAbilityInputID,TSubclassOf<UGameplayAbility>> BasicAbilities;
	
	UPROPERTY()
	bool StartUpAbilitiesGiven = false;
};
