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
	
	//** Gameplay Abilities Givers and Removers **/
	UFUNCTION(BlueprintCallable, Category="Abilities")
	FGameplayAbilitySpecHandle GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1);
	UFUNCTION(BlueprintCallable, Category="Abilities")
	void RemoveAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass);
	UFUNCTION(BlueprintCallable, Category="Abilities")
	void RemoveAllGrantedAbilities();
	
	//** Gameplay Abilities Bindings **/
	void BindAbilityToInputID(EHAbilityInputID InputID, TSubclassOf<UGameplayAbility> AbilityClass);
	void UnbindAbilityByInputID_Class(EHAbilityInputID InputID, TSubclassOf<UGameplayAbility> AbilityClass);
	void UnbindAllAbilitiesFromInputID(EHAbilityInputID InputID);
	
	const TMap<EHAbilityInputID, TSubclassOf<UGameplayAbility>>& GetBasicAbilities() const
	{
		return BasicAbilities;
	}

private:
	void HealthUpdated(const FOnAttributeChangeData& OnAttributeChangeData);

	UPROPERTY(EditAnywhere, Category="Hobo | Initial Effects", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UGameplayEffect> DeathEffect;
	
	UPROPERTY(EditAnywhere, Category="Hobo | Initial Effects", meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	UPROPERTY(EditAnywhere, Category="Hobo | Gameplay Abilities", meta=(AllowPrivateAccess="true"))
	TMap<EHAbilityInputID,TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditAnywhere, Category="Hobo | Gameplay BasicAbilities", meta=(AllowPrivateAccess="true"))
	TMap<EHAbilityInputID,TSubclassOf<UGameplayAbility>> BasicAbilities;
	
	UPROPERTY()
	bool StartUpAbilitiesGiven = false;
};
