// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoboLeagueProject/GAS/GameplayAbility/HBaseGameplayAbility.h"
#include "GA_Attack.generated.h"

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

	static FGameplayTag GetComboChangedEventTag();
	static FGameplayTag GetComboChangedEventEndTag();
	static FGameplayTag GetComboTargetEventTag();
	FGameplayTag GetComboUseStaminaEventTag();

private:

	void SetupWaitComboInputPress();

	UFUNCTION()
	void HandleInputPress(float TimeWaited);

	void TryCommitCombo();
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap;

	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultStaminaCost;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> StaminaCostMap;
	
	TSubclassOf<UGameplayEffect> GetStaminaEffectCostForCurrentCombo() const;

	UFUNCTION()
	void ConsumeStamina(FGameplayEventData Data);

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

	UFUNCTION()
	void GetComboChangedEventReceived(FGameplayEventData Data);
	bool CanPayStaminaForSection(FName SectionName) const;

	UFUNCTION()
	void DealDamage(FGameplayEventData Data);

	FName NextComboName;
};


