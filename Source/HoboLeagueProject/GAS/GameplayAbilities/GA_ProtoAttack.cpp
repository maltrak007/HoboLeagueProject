// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ProtoAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UGA_ProtoAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	if(!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if(HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, AttackMontage);
		PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_ProtoAttack::K2_EndAbility);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_ProtoAttack::K2_EndAbility);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_ProtoAttack::K2_EndAbility);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_ProtoAttack::K2_EndAbility);
		PlayMontageTask->ReadyForActivation();
	}
}
