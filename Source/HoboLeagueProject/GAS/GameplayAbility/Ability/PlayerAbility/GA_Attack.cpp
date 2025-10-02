// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Attack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "HoboLeagueProject/Item/PlayerItem/Weapon/HWeapon.h"


UGA_Attack::UGA_Attack() : AttackMontage(nullptr)
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

		UAbilityTask_WaitGameplayEvent* WaitStaminaEventTask =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetComboUseStaminaEventTag());
		WaitStaminaEventTask->EventReceived.AddDynamic(this, &UGA_Attack::ConsumeStamina);
		WaitStaminaEventTask->ReadyForActivation();
	}

	SetupWaitComboInputPress();
}

void UGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// Do not substitue this because is aiming to get only the prefix "Event.Combo.Change"
// and not the full tag "Event.Combo.Change.X" where X is the combo name
FGameplayTag UGA_Attack::GetComboChangedEventTag()
{
	return FGameplayTag::RequestGameplayTag("Event.Combo.Change");
}

FGameplayTag UGA_Attack::GetComboChangedEventEndTag()
{
	return FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_Change_End));
}

FGameplayTag UGA_Attack::GetComboTargetEventTag()
{
	return FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_Damage));
}

FGameplayTag UGA_Attack::GetComboUseStaminaEventTag()
{
	return FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_UseStamina));
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

TSubclassOf<UGameplayEffect> UGA_Attack::GetStaminaEffectCostForCurrentCombo() const
{
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (OwnerAnimInstance)
	{
		FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(AttackMontage);
		const TSubclassOf<UGameplayEffect>* FoundEffect = StaminaCostMap.Find(CurrentSectionName);
		if (FoundEffect) return *FoundEffect;
	}
	return DefaultStaminaCost;
}

void UGA_Attack::ConsumeStamina(FGameplayEventData Data)
{
	// Authority only — apply authoritative cost changes on the server.
	if (!K2_HasAuthority())
	{
		return;
	}

	TSubclassOf<UGameplayEffect> StaminaCostEffect = GetStaminaEffectCostForCurrentCombo();
	if (!StaminaCostEffect) return;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		// --- Build the outgoing spec (we'll apply this later if allowed) ---
		const int32 AbilityLevel = GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
		FGameplayEffectSpecHandle CostSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaCostEffect, AbilityLevel);

		if (!CostSpecHandle.IsValid())
		{
			return;
		}

		// --- IMPORTANT: Use CheckCost on the ability instance, but CheckCost inspects the ability's
		// CostGameplayEffectClass. Temporarily override it so CheckCost checks the section GE. ---
		TSubclassOf<UGameplayEffect> SavedCostGE = CostGameplayEffectClass; // store original

		CostGameplayEffectClass = StaminaCostEffect; // temporarily point ability's cost to section GE

		// Optional: collect tags that CheckCost returns (not required)
		FGameplayTagContainer OutRelevantTags;
		const bool bCanPay = CheckCost(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), &OutRelevantTags);

		// restore original cost GE immediately
		CostGameplayEffectClass = SavedCostGE;

		// CheckCost returns true when the cost CAN be paid
		if (!bCanPay)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough stamina for section (NextComboName = %s)"),
			       *NextComboName.ToString());
			// handle failure: cancel combo / end ability / play feedback, etc.
			K2_EndAbility();
			return;
		}

		// --- Apply the cost effect to self (server authoritative) ---
		// If your cost GE uses SetByCaller, set magnitudes on CostSpecHandle.Data here before applying.
		if (CostSpecHandle.IsValid() && CostSpecHandle.Data.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*CostSpecHandle.Data.Get());
		}
	}
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

		// ✅ Check stamina before accepting this combo
		if (!CanPayStaminaForSection(NextComboName))
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough stamina for combo section: %s"), *NextComboName.ToString());
			// Optionally cancel ability or just ignore input
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Next Combo is now :: %s"), *NextComboName.ToString());

		SetupWaitComboInputPress();
	}
}

bool UGA_Attack::CanPayStaminaForSection(FName SectionName) const
{
	TSubclassOf<UGameplayEffect> StaminaCostEffect = DefaultStaminaCost;

	if (const TSubclassOf<UGameplayEffect>* FoundEffect = StaminaCostMap.Find(SectionName))
	{
		StaminaCostEffect = *FoundEffect;
	}

	if (!StaminaCostEffect) return true; // no GE means no cost

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		const int32 AbilityLevel = GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
		FGameplayEffectSpecHandle CostSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaCostEffect, AbilityLevel);

		if (!CostSpecHandle.IsValid())
		{
			return false;
		}

		// Temporarily override ability's cost GE for CheckCost
		TSubclassOf<UGameplayEffect> SavedCostGE = CostGameplayEffectClass;
		const_cast<UGA_Attack*>(this)->CostGameplayEffectClass = StaminaCostEffect;

		FGameplayTagContainer OutTags;
		const bool bCanPay = CheckCost(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), &OutTags);

		const_cast<UGA_Attack*>(this)->CostGameplayEffectClass = SavedCostGE;

		return bCanPay;
	}

	return false;
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
		                                EffectSpecHandle,
		                                UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(
			                                HitResult.GetActor()));

		const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
		
		APlayerCharacter* PC = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
		
		if (AHWeapon* Weapon = Cast<AHWeapon>(PC->GetInventoryComponent()->GetEquippedItem()))
		{
			Weapon->ReduceWeaponDurability();
			Weapon->OnWeaponHit.Broadcast();
		}
	}
}
