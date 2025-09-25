// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Attack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"


UGA_Attack::UGA_Attack(): AttackMontage(nullptr)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, NAME_None, AttackMontage);
		PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_Attack::K2_EndAbility);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_Attack::K2_EndAbility);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_Attack::K2_EndAbility);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_Attack::K2_EndAbility);
		PlayMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			GetComboChangedEventTag(), nullptr, false, false);

		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &UGA_Attack::GetComboChangedEventReceived);
		WaitComboChangeEventTask->ReadyForActivation();
	}

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
			GetComboTargetEventTag());
		WaitTargetingEventTask->EventReceived.AddDynamic(this, &UGA_Attack::DealDamage);
		WaitTargetingEventTask->ReadyForActivation();
	}

	SetupWaitComboInputPress();
}

FGameplayTag UGA_Attack::GetComboChangedEventTag()
{
	return FGameplayTag::RequestGameplayTag("Event.Combo.Change");
}

FGameplayTag UGA_Attack::GetComboChangedEventEndTag()
{
	return FGameplayTag::RequestGameplayTag("Event.Combo.Change.End");
}

FGameplayTag UGA_Attack::GetComboTargetEventTag()
{
	return FGameplayTag::RequestGameplayTag("Event.Combo.Damage");
}

void UGA_Attack::SetupWaitComboInputPress()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &UGA_Attack::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_Attack::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();
	TryCommitCombo();
}

void UGA_Attack::TryCommitCombo()
{
	if (NextComboName == NAME_None)
	{
		return;
	}

	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();

	if (!OwnerAnimInstance)
	{
		return;
	}
	OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(AttackMontage),
	                                          NextComboName, AttackMontage);
}

TSubclassOf<UGameplayEffect> UGA_Attack::GetDamageEffectForCurrentCombo() const
{
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (OwnerAnimInstance)
	{
		FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(AttackMontage);
		const TSubclassOf<UGameplayEffect>* FoundEffect = DamageEffectMap.Find(CurrentSectionName);
		if (FoundEffect) return *FoundEffect;
	}
	return DefaultDamageEffect;
}

void UGA_Attack::GetComboChangedEventReceived(FGameplayEventData Data)
{
	FGameplayTag EventTag = Data.EventTag;
	if (EventTag == GetComboChangedEventEndTag())
	{
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("Next combo is cleared"));
		return;
	}

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	if (TagNames.Num() > 0)
	{
		NextComboName = TagNames.Last();

		UE_LOG(LogTemp, Warning, TEXT("Next Combo is now :: %s"), *NextComboName.ToString());

		SetupWaitComboInputPress();
	}
}

void UGA_Attack::DealDamage(FGameplayEventData Data)
{
	TArray<FHitResult> HitResults = GetHitResultFromSweepLocationTargetData(Data.TargetData, 30.f, true, true);
	for (const FHitResult& HitResult : HitResults)
	{
		TSubclassOf<UGameplayEffect> DamageEffect = GetDamageEffectForCurrentCombo();
		
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(
			DamageEffect, GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()));
		ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo,
		                                EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor()));
	}
}
