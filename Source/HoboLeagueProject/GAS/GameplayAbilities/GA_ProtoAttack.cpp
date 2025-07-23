// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ProtoAttack.h"

#include "GameplayTagsManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"


void UGA_ProtoAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if(HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		//TODO Explore, K2, WTF mens, come on dudes
		if(!K2_CommitAbility())
		{
			K2_EndAbility();
		}
		
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, AttackMontage);
		PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_ProtoAttack::K2_EndAbility);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_ProtoAttack::K2_EndAbility);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_ProtoAttack::K2_EndAbility);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_ProtoAttack::K2_EndAbility);
		PlayMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,GetComboChangedEventTag(),nullptr,false,false);
		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &UGA_ProtoAttack::GetComboChangedEventRecived);
		WaitComboChangeEventTask->ReadyForActivation();
	}
	SetupWaitComboInputPress();
}

FGameplayTag UGA_ProtoAttack::GetComboChangedEventTag()
{
	return FGameplayTag::RequestGameplayTag("Event.Combo.Change");
}

FGameplayTag UGA_ProtoAttack::GetComboChangedEventEndTag()
{
	return FGameplayTag::RequestGameplayTag("Event.Combo.Change.End");
}

void UGA_ProtoAttack::SetupWaitComboInputPress()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this,&UGA_ProtoAttack::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_ProtoAttack::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();
	TryCommitCombo();
}

void UGA_ProtoAttack::TryCommitCombo()
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
	OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(AttackMontage), NextComboName,AttackMontage);
	
}

void UGA_ProtoAttack::GetComboChangedEventRecived(FGameplayEventData Data)
{
	FGameplayTag EventTag = Data.EventTag;
	if (EventTag == GetComboChangedEventEndTag())
	{
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("next combo is cleared"));
		return;
	}
	

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	NextComboName = TagNames.Last();
	
	UE_LOG(LogTemp, Warning, TEXT("next combo is now: %s"), *NextComboName.ToString());

}
