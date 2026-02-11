// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Attack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagsManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"

UGA_Attack::UGA_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// Configure blocking/canceling tags as needed
	// ActivationBlockedTags.AddTag(...);
	// CancelAbilitiesWithTag.AddTag(...);
}

void UGA_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// Commit ability (check costs, cooldowns, etc.)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Validate montage
	if (!AttackMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_Attack: No attack montage assigned!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Reset combo state
	NextComboName = NAME_None;
	bIsWaitingForComboInput = false;

	// Setup tasks based on authority
	SetupMontageTask();
	SetupEventTasks();
	SetupInputTask();

	UE_LOG(LogTemp, Log, TEXT("[%s] Attack ability activated"),
		HasAuthority(&ActivationInfo) ? TEXT("SERVER") : TEXT("CLIENT"));
}

void UGA_Attack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// Clear combo state
	NextComboName = NAME_None;
	bIsWaitingForComboInput = false;

	// Clear task references (base class handles cancellation)
	MontageTask = nullptr;
	InputTask = nullptr;
	ComboChangeTask = nullptr;
	DamageTask = nullptr;
	StaminaTask = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_LOG(LogTemp, Log, TEXT("[%s] Attack ability ended (Cancelled: %s)"),
		HasAuthority(&ActivationInfo) ? TEXT("SERVER") : TEXT("CLIENT"),
		bWasCancelled ? TEXT("YES") : TEXT("NO"));
}

// ============ Setup Functions ============

void UGA_Attack::SetupMontageTask()
{
	// Create montage task (runs on both client and server for prediction)
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		AttackMontage,
		1.0f // Play rate
	);

	if (MontageTask)
	{
		// Bind all montage events
		MontageTask->OnCompleted.AddDynamic(this, &UGA_Attack::OnMontageCompleted);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_Attack::OnMontageCancelled);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_Attack::OnMontageInterrupted);
		MontageTask->OnBlendOut.AddDynamic(this, &UGA_Attack::OnMontageBlendOut);

		RegisterTask(MontageTask);
		MontageTask->ReadyForActivation();
	}
}

void UGA_Attack::SetupEventTasks()
{
	// Combo change event (client and server)
	ComboChangeTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		GetComboChangeEventTag(),
		nullptr, // Any source
		false, // OnlyTriggerOnce
		false  // OnlyMatchExact (allows child tags)
	);

	if (ComboChangeTask)
	{
		ComboChangeTask->EventReceived.AddDynamic(this, &UGA_Attack::OnComboChangeEvent);
		RegisterTask(ComboChangeTask);
		ComboChangeTask->ReadyForActivation();
	}

	// Server-only tasks
	if (HasAuthority(&CurrentActivationInfo))
	{
		// Damage event
		DamageTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			GetDamageEventTag()
		);

		if (DamageTask)
		{
			DamageTask->EventReceived.AddDynamic(this, &UGA_Attack::OnDamageEvent);
			RegisterTask(DamageTask);
			DamageTask->ReadyForActivation();
		}

		// Stamina cost event
		StaminaTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			GetStaminaEventTag()
		);

		if (StaminaTask)
		{
			StaminaTask->EventReceived.AddDynamic(this, &UGA_Attack::OnStaminaEvent);
			RegisterTask(StaminaTask);
			StaminaTask->ReadyForActivation();
		}
	}
}

void UGA_Attack::SetupInputTask()
{
	// Cancel existing input task if any
	if (InputTask && InputTask->IsActive())
	{
		InputTask->ExternalCancel();
	}

	// Create new input task
	InputTask = UAbilityTask_WaitInputPress::WaitInputPress(this, false);

	if (InputTask)
	{
		InputTask->OnPress.AddDynamic(this, &UGA_Attack::OnComboInputPressed);
		RegisterTask(InputTask);
		InputTask->ReadyForActivation();
		
		bIsWaitingForComboInput = true;
	}
}

// ============ Montage Callbacks ============

void UGA_Attack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Attack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Attack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Attack::OnMontageBlendOut()
{
	// Optional: Could end here or wait for OnCompleted
	// EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

// ============ Input & Combo Logic ============

void UGA_Attack::OnComboInputPressed(float TimeWaited)
{
	UE_LOG(LogTemp, Log, TEXT("Combo input pressed (Time: %.2f)"), TimeWaited);

	// Try to commit the queued combo
	TryCommitCombo();

	// Setup new input task for next combo window
	SetupInputTask();
}

void UGA_Attack::OnComboChangeEvent(FGameplayEventData Data)
{
	const FGameplayTag EventTag = Data.EventTag;

	// Check if this is the end combo event
	if (EventTag.MatchesTagExact(GetComboEndEventTag()))
	{
		NextComboName = NAME_None;
		bIsWaitingForComboInput = false;
		
		UE_LOG(LogTemp, Log, TEXT("Combo chain ended"));
		return;
	}

	// Extract combo section name from tag
	// Example: "Event.Combo.Change.Attack2" -> "Attack2"
	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);

	if (TagNames.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to extract combo name from tag: %s"), 
			*EventTag.ToString());
		return;
	}

	const FName PotentialComboName = TagNames.Last();

	// Validate that this combo section exists in montage
	if (!AttackMontage->IsValidSectionName(PotentialComboName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid combo section: %s"), *PotentialComboName.ToString());
		return;
	}

	// Check stamina before accepting combo
	if (!CanPayStaminaForSection(PotentialComboName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough stamina for combo: %s"), 
			*PotentialComboName.ToString());
		
		// Optional: Play "out of stamina" feedback here
		return;
	}

	// Accept the combo
	NextComboName = PotentialComboName;
	
	UE_LOG(LogTemp, Log, TEXT("Next combo queued: %s"), *NextComboName.ToString());
}

void UGA_Attack::TryCommitCombo()
{
	if (NextComboName == NAME_None)
	{
		UE_LOG(LogTemp, Log, TEXT("No combo queued"));
		return;
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (!AnimInstance || !AttackMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot commit combo - invalid anim instance or montage"));
		return;
	}

	// Double-check montage is playing
	if (!AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot commit combo - montage not playing"));
		return;
	}

	// Get current section
	const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(AttackMontage);

	// Commit combo by setting next section
	AnimInstance->Montage_SetNextSection(CurrentSection, NextComboName, AttackMontage);

	UE_LOG(LogTemp, Log, TEXT("Combo committed: %s -> %s"), 
		*CurrentSection.ToString(), *NextComboName.ToString());

	// Clear queued combo
	NextComboName = NAME_None;
}

// ============ Stamina Management ============

bool UGA_Attack::CanPayStaminaForSection(FName SectionName) const
{
	// Get stamina cost for this section
	TSubclassOf<UGameplayEffect> StaminaCost = DefaultStaminaCost;

	if (const TSubclassOf<UGameplayEffect>* FoundCost = StaminaCostMap.Find(SectionName))
	{
		StaminaCost = *FoundCost;
	}

	// No cost = always can afford
	if (!StaminaCost)
	{
		return true;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}

	// Create a temporary spec to check cost
	const FGameplayEffectSpecHandle SpecHandle = 
		ASC->MakeOutgoingSpec(StaminaCost, GetCurrentAbilityLevel(), ASC->MakeEffectContext());

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	// Check if we can afford it
	return ASC->CanApplyAttributeModifiers(
		StaminaCost.GetDefaultObject(),
		GetCurrentAbilityLevel(),
		ASC->MakeEffectContext()
	);
}

void UGA_Attack::OnStaminaEvent(FGameplayEventData Data)
{
	// Server only
	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	ConsumeStamina();
}

void UGA_Attack::ConsumeStamina()
{
	TSubclassOf<UGameplayEffect> StaminaCost = GetStaminaCostForCurrentCombo();
	
	if (!StaminaCost)
	{
		return; // No cost for this section
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	// Create and apply stamina cost effect
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
		StaminaCost,
		GetCurrentAbilityLevel()
	);

	if (SpecHandle.IsValid())
	{
		// Apply to self
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		UE_LOG(LogTemp, Log, TEXT("Stamina consumed for current combo section"));
	}
}

TSubclassOf<UGameplayEffect> UGA_Attack::GetStaminaCostForCurrentCombo() const
{
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (!AnimInstance || !AttackMontage)
	{
		return DefaultStaminaCost;
	}

	const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(AttackMontage);
	
	if (const TSubclassOf<UGameplayEffect>* FoundCost = StaminaCostMap.Find(CurrentSection))
	{
		return *FoundCost;
	}

	return DefaultStaminaCost;
}

// ============ Damage Dealing ============

void UGA_Attack::OnDamageEvent(FGameplayEventData Data)
{
	// Server only
	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	// Validate target data
	if (!Data.TargetData.IsValid(0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Damage event received with invalid target data"));
		return;
	}

	DealDamage(Data.TargetData);
}

void UGA_Attack::DealDamage(const FGameplayAbilityTargetDataHandle& TargetData)
{
	// Get damage effect for current combo section
	TSubclassOf<UGameplayEffect> DamageEffect = GetDamageEffectForCurrentCombo();
	
	if (!DamageEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("No damage effect configured for current combo"));
		return;
	}

	// Perform sweep to find hit targets
	TArray<FHitResult> HitResults = GetHitResultFromSweepLocationTargetData(
		TargetData,
		SphereRadiusSweep,
		bDebugDrawHits,
		true // Ignore self
	);

	if (HitResults.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("No targets hit"));
		return;
	}

	// Create damage effect spec
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
		DamageEffect,
		GetCurrentAbilityLevel()
	);

	if (!SpecHandle.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC)
	{
		return;
	}

	// Apply damage to each hit target
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor))
		{
			continue;
		}
		UAbilitySystemComponent* TargetASC = nullptr;
		if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(HitActor))
		{
			TargetASC = ASCInterface->GetAbilitySystemComponent();
		}

		if (!TargetASC)
		{
			UE_LOG(LogTemp, Log, TEXT("Hit actor has no ASC: %s"), *GetNameSafe(HitActor));
			continue;
		}

		// Apply damage
		SourceASC->ApplyGameplayEffectSpecToTarget(
			*SpecHandle.Data.Get(),
			TargetASC
		);

		UE_LOG(LogTemp, Log, TEXT("Damage applied to: %s"), *GetNameSafe(HitActor));
	}

	// Optional: Reduce weapon durability
	// This should be done through a separate system, not hardcoded here
	// See improved architecture in the analysis document
}

TSubclassOf<UGameplayEffect> UGA_Attack::GetDamageEffectForCurrentCombo() const
{
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (!AnimInstance || !AttackMontage)
	{
		return DefaultDamageEffect;
	}

	const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(AttackMontage);
	
	if (const TSubclassOf<UGameplayEffect>* FoundEffect = DamageEffectMap.Find(CurrentSection))
	{
		return *FoundEffect;
	}

	return DefaultDamageEffect;
}

// ============ Gameplay Tag Helpers ============

FGameplayTag UGA_Attack::GetComboChangeEventTag() const
{
	// Use prefix so we catch all combo change events
	return FGameplayTag::RequestGameplayTag("Event.Combo.Change");
}

FGameplayTag UGA_Attack::GetComboEndEventTag() const
{
	return FGameplayTag::RequestGameplayTag(
		FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_Change_End)
	);
}

FGameplayTag UGA_Attack::GetDamageEventTag() const
{
	return FGameplayTag::RequestGameplayTag(
		FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_Damage)
	);
}

FGameplayTag UGA_Attack::GetStaminaEventTag() const
{
	return FGameplayTag::RequestGameplayTag(
		FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_UseStamina)
	);
}