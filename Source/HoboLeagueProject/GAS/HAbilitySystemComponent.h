// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HGameplayAbilityTypes.h"
#include "HAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOBOLEAGUEPROJECT_API UHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UHAbilitySystemComponent();
	
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	
	//** Gameplay Abilities Givers **/
	UFUNCTION(BlueprintCallable, Category="Abilities")
	TArray<FGameplayAbilitySpecHandle> GrantAbility(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant, int32 Level = 1);
	
	UFUNCTION(BlueprintCallable, Category="Abilities")
	void RemoveGrantedAbility(TArray<FGameplayAbilitySpecHandle> Handle);

protected:
	
	void OnRep_ActivateAbilities() override;
	
private:
	// ---------- Attribute Change Callbacks ---------- //
	void HealthUpdated(const FOnAttributeChangeData& OnAttributeChangeData);
	
	void StaminaUpdated(const FOnAttributeChangeData& OnAttributeChangeData);
	
	void OverdoseUpdated(const FOnAttributeChangeData& OnAttributeChangeData);

	// --------- Gameplay Effects needed for callbacks-------------- //
	UPROPERTY(EditAnywhere, Category="Hobo | Initial Effects", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UGameplayEffect> DeathEffect;

	UPROPERTY(EditAnywhere, Category="Hobo | Initial Effects", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UGameplayEffect> OverdoseEffect;
	
	// --------- Gameplay Effects -------------- //
	UPROPERTY(EditAnywhere, Category="Hobo | Initial Effects", meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	UPROPERTY(EditAnywhere, Category="Hobo | Gameplay Abilities", meta=(AllowPrivateAccess="true"))
	TMap<EHAbilityInputID,TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditAnywhere, Category="Hobo | Gameplay Abilities", meta=(AllowPrivateAccess="true"))
	TMap<EHAbilityInputID,TSubclassOf<UGameplayAbility>> BasicAbilities;

	UPROPERTY(EditAnywhere, Category="Hobo | Gameplay Abilities", meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
	
	UPROPERTY()
	bool StartUpAbilitiesGiven = false;
};
