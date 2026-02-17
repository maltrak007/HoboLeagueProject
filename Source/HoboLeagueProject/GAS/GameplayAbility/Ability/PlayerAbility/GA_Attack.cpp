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

	// Default to hybrid mode (both hold and press work)
	bAllowAutoCombo = true;
	bAllowManualCombo = true;
	bManualPressTakesPriority = false;
	ComboWindowDuration = 0.5f;
}

void UGA_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// Commit ability (check costs, cooldowns)
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

	// Reset state
	NextComboName = NAME_None;
	
	if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		bIsInputHeld = Spec->InputPressed;
	}
	
	if (ActorInfo->IsLocallyControlled() && !bIsInputHeld)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client started attack but input is not held. Stopping auto-combo."));
	}
	
	bManualPressReceived = false;
	bInComboWindow = false;

	// Clear any existing timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ComboWindowTimer);
	}

	// Setup tasks
	SetupMontageTask();
	SetupEventTasks();
	
	// Only setup input task if manual combo is allowed
	if (bAllowManualCombo)
	{
		SetupInputTask();
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] Attack activated - AutoCombo: %s, ManualCombo: %s"),
		HasAuthority(&ActivationInfo) ? TEXT("SERVER") : TEXT("CLIENT"),
		bAllowAutoCombo ? TEXT("ON") : TEXT("OFF"),
		bAllowManualCombo ? TEXT("ON") : TEXT("OFF"));
}

void UGA_Attack::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	// Player released attack button
	bIsInputHeld = false;
	

	UE_LOG(LogTemp, Log, TEXT("Attack input released - auto-combo disabled"));

	// Don't end ability here - let animation finish
	// If manual combo is enabled, player can still press to continue
}

void UGA_Attack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// Clear combo window timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ComboWindowTimer);
	}
	
	bShouldChainAttack = ShouldChainAttack();
	
	// Clear state
	NextComboName = NAME_None;
	bIsInputHeld = false;
	bManualPressReceived = false;
	bInComboWindow = false;

	// Clear task references (base class handles cancellation)
	MontageTask = nullptr;
	InputTask = nullptr;
	ComboChangeTask = nullptr;
	DamageTask = nullptr;
	StaminaTask = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_LOG(LogTemp, Log, TEXT("[%s] Attack ended (Cancelled: %s)"),
		HasAuthority(&ActivationInfo) ? TEXT("SERVER") : TEXT("CLIENT"),
		bWasCancelled ? TEXT("YES") : TEXT("NO"));
	
	if (bShouldChainAttack && !bWasCancelled)
	{
		// Use ASC to try activating the ability again
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			// Try to activate by spec handle
			bool bActivated = ASC->TryActivateAbility(Handle, true);
			
			if (bActivated)
			{
				UE_LOG(LogTemp, Log, TEXT("✓ Successfully chained to new attack!"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to chain attack - ability activation failed"));
			}
		}
	}
}

// ============ Setup Functions ============

void UGA_Attack::SetupMontageTask()
{
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
	// Combo change event (both client and server need this for animations)
	ComboChangeTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		GetComboChangeEventTag(),
		nullptr, // Any source
		false,   // OnlyTriggerOnce
		false    // OnlyMatchExact (allows child tags like Event.Combo.Change.Attack2)
	);

	if (ComboChangeTask)
	{
		ComboChangeTask->EventReceived.AddDynamic(this, &UGA_Attack::OnComboChangeEvent);
		RegisterTask(ComboChangeTask);
		ComboChangeTask->ReadyForActivation();
	}

	// Server-only events
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

	// Create new input task to wait for manual presses
	InputTask = UAbilityTask_WaitInputPress::WaitInputPress(this, false);

	if (InputTask)
	{
		InputTask->OnPress.AddDynamic(this, &UGA_Attack::OnManualComboInput);
		RegisterTask(InputTask);
		InputTask->ReadyForActivation();
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
	// Keeping it to complete naturally
}

// ============ Input Callbacks ============

void UGA_Attack::OnManualComboInput(float TimeWaited)
{
	UE_LOG(LogTemp, Log, TEXT("Manual combo input pressed (Time waited: %.2f)"), TimeWaited);

	// Mark that player manually pressed
	bManualPressReceived = true;

	// If we're in a combo window, try to commit the combo
	if (bInComboWindow && NextComboName != NAME_None)
	{
		TryCommitCombo();
	}

	// Setup new input task for next press
	if (bAllowManualCombo)
	{
		SetupInputTask();
	}
}

// ============ Event Callbacks ============

void UGA_Attack::OnComboChangeEvent(FGameplayEventData Data)
{
	const FGameplayTag EventTag = Data.EventTag;

	// Check if this is the combo end event
	if (EventTag.MatchesTagExact(GetComboEndEventTag()))
	{
		NextComboName = NAME_None;
		bInComboWindow = false;
		
		// Clear combo window timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ComboWindowTimer);
		}

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

	// Validate section exists in montage
	if (!AttackMontage->IsValidSectionName(PotentialComboName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid combo section: %s"), *PotentialComboName.ToString());
		return;
	}

	// Check stamina cost
	if (!CanPayStaminaForSection(PotentialComboName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough stamina for combo: %s"), 
			*PotentialComboName.ToString());
		
		// Force end combo if can't afford it
		NextComboName = NAME_None;
		bInComboWindow = false;
		bIsInputHeld = false; // Stop auto-combo
		return;
	}

	// Accept the combo
	NextComboName = PotentialComboName;
	bInComboWindow = true;
	bManualPressReceived = false; // Reset manual press flag for this window

	UE_LOG(LogTemp, Log, TEXT("Combo window opened: %s (Input held: %s)"),
		*NextComboName.ToString(),
		bIsInputHeld ? TEXT("YES") : TEXT("NO"));

	// Start combo window timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ComboWindowTimer,
			this,
			&UGA_Attack::OnComboWindowExpired,
			ComboWindowDuration,
			false
		);
	}
	
	// Priority 1: Manual press (if enabled and configured to take priority)
	if (bManualPressTakesPriority && bAllowManualCombo)
	{
		// Wait for manual press - do nothing here
		UE_LOG(LogTemp, Log, TEXT("Waiting for manual press (priority mode)"));
		return;
	}

	// Priority 2: Auto-combo (if enabled and input is held)
	if (bAllowAutoCombo && bIsInputHeld)
	{
		UE_LOG(LogTemp, Log, TEXT("Auto-combo triggered (input held)"));
		TryCommitCombo();
		return;
	}

	// Priority 3: Wait for manual press (if enabled)
	if (bAllowManualCombo)
	{
		UE_LOG(LogTemp, Log, TEXT("Waiting for manual press"));
		return;
	}

	// No valid combo method - window will expire
	UE_LOG(LogTemp, Warning, TEXT("No valid combo method available"));
}

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

void UGA_Attack::OnStaminaEvent(FGameplayEventData Data)
{
	// Server only
	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	ConsumeStamina();
}

// ============ Combo Logic ============

void UGA_Attack::TryCommitCombo()
{
	if (NextComboName == NAME_None)
	{
		UE_LOG(LogTemp, Log, TEXT("No combo queued to commit"));
		return;
	}
	
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (!AnimInstance || !AttackMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot commit combo - invalid anim instance or montage"));
		return;
	}

	// Verify montage is still playing
	if (!AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot commit combo - montage not playing"));
		return;
	}

	// Get current section
	const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(AttackMontage);

	// Commit the combo by setting next section
	AnimInstance->Montage_SetNextSection(CurrentSection, NextComboName, AttackMontage);

	UE_LOG(LogTemp, Log, TEXT("✓ COMBO COMMITTED: %s -> %s (Method: %s)"),
		*CurrentSection.ToString(),
		*NextComboName.ToString(),
		bManualPressReceived ? TEXT("MANUAL PRESS") : TEXT("AUTO-HOLD"));

	// Clear combo state
	NextComboName = NAME_None;
	bInComboWindow = false;
	bManualPressReceived = false;

	// Clear combo window timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ComboWindowTimer);
	}
}

void UGA_Attack::OnComboWindowExpired()
{
	UE_LOG(LogTemp, Log, TEXT("Combo window expired - combo chain will end"));

	// Clear combo state
	NextComboName = NAME_None;
	bInComboWindow = false;
	bManualPressReceived = false;

	// Combo chain will naturally end after current attack completes
}

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

	// Use ASC's direct cost checking
	return ASC->CanApplyAttributeModifiers(
		StaminaCost.GetDefaultObject(),
		GetCurrentAbilityLevel(),
		ASC->MakeEffectContext()
	);
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
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		
		UE_LOG(LogTemp, Log, TEXT("Stamina consumed for combo section"));
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

		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(HitActor);
		if (!ASI) { UE_LOG(LogTemp, Log, TEXT("Hit actor does not implement IAbilitySystemInterface: %s"), *GetNameSafe(HitActor)); continue; }
		UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();

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

		UE_LOG(LogTemp, Log, TEXT("✓ Damage applied to: %s at location: %s"), 
			*GetNameSafe(HitActor),
			*Hit.Location.ToString());
	}
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
	// Use prefix to catch all combo change events
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

bool UGA_Attack::ShouldChainAttack() const
{
	// Only chain if:
	// 1. Input is still held (player hasn't released)
	// 2. Auto-combo is enabled
	// 3. We're not in a combo window (finished the combo chain)
	
	if (!IsLocallyControlled())
	{
		return false; 
	}

	if (!bIsInputHeld || !bAllowAutoCombo || bInComboWindow)
	{
		return false;
	}
	
	if (!CanPayStaminaForSection(FirstComboSection))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot chain - not enough stamina"));
		return false;
	}

	return true;
}