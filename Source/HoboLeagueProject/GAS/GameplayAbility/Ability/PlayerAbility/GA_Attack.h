// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_Attack.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UGA_Attack : public UHBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Attack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	static FGameplayTag GetComboChangedEventTag();
	
	static FGameplayTag GetComboChangedEventEndTag();
	
	static FGameplayTag GetComboTargetEventTag();
	
	FGameplayTag GetComboUseStaminaEventTag();
	
private:
	void SetupWaitComboInputPress();

	UFUNCTION()
	void HandleInputPress(float TimeWaited);

	void TryCommitCombo();
	
	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const;
	
	TSubclassOf<UGameplayEffect> GetStaminaEffectCostForCurrentCombo() const;

	UFUNCTION()
	void ConsumeStamina(FGameplayEventData Data);
	
	bool CanPayStaminaForSection(FName SectionName) const;
	
	UFUNCTION()
	void GetComboChangedEventReceived(FGameplayEventData Data);
	
	UFUNCTION()
	void DealDamage(FGameplayEventData Data);
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

	FName NextComboName;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float SphereRadiusSweep = 30.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultStaminaCost;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> StaminaCostMap;
};


