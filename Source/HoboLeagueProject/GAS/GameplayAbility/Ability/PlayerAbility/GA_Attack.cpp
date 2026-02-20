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
#include "HoboLeagueProject/Item/PlayerItem/Weapon/HWeaponStatsTable.h"

static TAutoConsoleVariable<int32> CVarAttackDebugLog(
    TEXT("ga.Attack.DebugLog"),
    0,
    TEXT("Enable debug logging for GA_Attack (0=off, 1=on)"),
    ECVF_Default
);

#define ATTACK_LOG(Verbosity, Format, ...) \
if (CVarAttackDebugLog.GetValueOnGameThread() > 0) \
{ \
UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__); \
}


UGA_Attack::UGA_Attack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

    bAllowAutoCombo = true;
    bAllowManualCombo = true;
    bManualPressTakesPriority = false;
    ComboWindowDuration = 0.5f;

    
    
    CachedIgnoreActors.Reserve(10);
}

void UGA_Attack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    CacheAbilityData();
    if (!CachedWeaponStats)
    {
        ATTACK_LOG(Error, TEXT("GA_Attack: No weapon stats available!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CanPayStaminaForAttack())
    {
        ATTACK_LOG(Warning, TEXT("GA_Attack: Not enough stamina to attack"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!AttackMontage)
    {
        ATTACK_LOG(Error, TEXT("GA_Attack: No attack montage assigned!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Reset state
    CurrentEventIgnoredActors.Empty();
    NextComboName = NAME_None;
    bManualPressReceived = false;
    bInComboWindow = false;

    // Check input state
    if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
    {
        bIsInputHeld = Spec->InputPressed;
    }

    // Clear any existing timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ComboWindowTimer);
    }

    // Setup tasks
    SetupMontageTask();
    SetupEventTasks();

    if (bAllowManualCombo)
    {
        SetupInputTask();
    }

    ATTACK_LOG(Log, TEXT("[%s] Attack activated - Weapon: %s, Rarity: %s, Damage Mods: %d"),
        HasAuthority(&ActivationInfo) ? TEXT("SERVER") : TEXT("CLIENT"),
        CachedWeapon ? *CachedWeapon->GetName() : TEXT("None"),
        CachedWeapon ? *UEnum::GetValueAsString(CachedWeapon->ItemRarity) : TEXT("None"),
        CachedWeaponStats->DamageModifiers.Num());
}

void UGA_Attack::InputReleased(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);

    bIsInputHeld = false;
    UE_LOG(LogTemp, Log, TEXT("Attack input released - auto-combo disabled"));
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
    CurrentEventIgnoredActors.Empty();
    NextComboName = NAME_None;
    bIsInputHeld = false;
    bManualPressReceived = false;
    bInComboWindow = false;

    ClearCachedData();
    
    // Clear task references
    MontageTask = nullptr;
    InputTask = nullptr;
    ComboChangeTask = nullptr;
    DamageTask = nullptr;
    StaminaTask = nullptr;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    UE_LOG(LogTemp, Log, TEXT("[%s] Attack ended (Cancelled: %s)"),
        HasAuthority(&ActivationInfo) ? TEXT("SERVER") : TEXT("CLIENT"),
        bWasCancelled ? TEXT("YES") : TEXT("NO"));

    // Chain to next attack if conditions met
    if (bShouldChainAttack && !bWasCancelled)
    {
        if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
        {
            if (ASC->TryActivateAbility(Handle, true))
            {
                ATTACK_LOG(Log, TEXT("✓ Chained to new attack!"));
            }
        }
    }
}

void UGA_Attack::CacheAbilityData()
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
    if (!Player) { return; }

    UHInventoryComponent* Inventory = Player->GetInventoryComponent();
    if (!Inventory) { return; }

    CachedWeapon = Inventory->GetActiveWeapon();
    if (CachedWeapon)
    {
        CachedWeaponStats = CachedWeapon->GetWeaponStats();
    }

    CachedAnimInstance = GetOwnerAnimInstance();

    Tag_ComboChange = FGameplayTag::RequestGameplayTag(
        FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_Change)
    );
    Tag_ComboEnd = FGameplayTag::RequestGameplayTag(
        FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_Change_End)
    );
    Tag_Damage = FGameplayTag::RequestGameplayTag(
        FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_Damage)
    );
    Tag_Stamina = FGameplayTag::RequestGameplayTag(
        FHGameplayTags::GetTagName(FHGameplayTags::Get().Event_Combo_UseStamina)
    );
    Tag_StaminaCost = FGameplayTag::RequestGameplayTag(
        FHGameplayTags::GetTagName(FHGameplayTags::Get().Data_StaminaCost)
    );
}

void UGA_Attack::SetupMontageTask()
{
    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,
        AttackMontage,
        1.0f
    );

    if (MontageTask)
    {
        MontageTask->OnCompleted.AddDynamic(this, &UGA_Attack::OnMontageCompleted);
        MontageTask->OnCancelled.AddDynamic(this, &UGA_Attack::OnMontageCancelled);
        MontageTask->OnInterrupted.AddDynamic(this, &UGA_Attack::OnMontageInterrupted);
        MontageTask->OnBlendOut.AddDynamic(this, &UGA_Attack::OnMontageBlendOut);

        MontageTask->ReadyForActivation();
    }
}

void UGA_Attack::SetupEventTasks()
{
    // Combo change event (client + server for animations)
    ComboChangeTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this,
        GetComboChangeEventTag(),
        nullptr,
        false,
        false
    );

    if (ComboChangeTask)
    {
        ComboChangeTask->EventReceived.AddDynamic(this, &UGA_Attack::OnComboChangeEvent);
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

    InputTask = UAbilityTask_WaitInputPress::WaitInputPress(this, false);

    if (InputTask)
    {
        InputTask->OnPress.AddDynamic(this, &UGA_Attack::OnManualComboInput);
        InputTask->ReadyForActivation();
    }
}

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
    // Do nothing - let montage complete naturally
}

void UGA_Attack::OnManualComboInput(float TimeWaited)
{
    UE_LOG(LogTemp, Log, TEXT("Manual combo input pressed (Time: %.2f)"), TimeWaited);

    bManualPressReceived = true;

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

void UGA_Attack::OnComboChangeEvent(FGameplayEventData Data)
{
    const FGameplayTag EventTag = Data.EventTag;

    // Check if combo chain ended
    if (EventTag.MatchesTagExact(GetComboEndEventTag()))
    {
        NextComboName = NAME_None;
        bInComboWindow = false;

        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(ComboWindowTimer);
        }

        UE_LOG(LogTemp, Log, TEXT("Combo chain ended"));
        return;
    }
    
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
        UE_LOG(LogTemp, Warning, TEXT("Invalid combo section: %s"),
            *PotentialComboName.ToString());
        return;
    }

    // Check stamina cost
    if (!CanPayStaminaForAttack())
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough stamina for combo: %s"),
            *PotentialComboName.ToString());

        // Force end combo
        NextComboName = NAME_None;
        bInComboWindow = false;
        bIsInputHeld = false;
        return;
    }

    // Accept the combo
    NextComboName = PotentialComboName;
    bInComboWindow = true;
    bManualPressReceived = false;

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

    // Priority 1: Manual press (if enabled and takes priority)
    if (bManualPressTakesPriority && bAllowManualCombo)
    {
        UE_LOG(LogTemp, Log, TEXT("Waiting for manual press (priority mode)"));
        return;
    }

    // Priority 2: Auto-combo (if enabled and input held)
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
        UE_LOG(LogTemp, Error, TEXT("Cannot commit combo - invalid anim instance/montage"));
        return;
    }

    if (!AnimInstance->Montage_IsPlaying(AttackMontage))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot commit combo - montage not playing"));
        return;
    }

    const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(AttackMontage);

    // Commit the combo
    AnimInstance->Montage_SetNextSection(CurrentSection, NextComboName, AttackMontage);

    UE_LOG(LogTemp, Log, TEXT("✓ COMBO COMMITTED: %s -> %s (Method: %s)"),
        *CurrentSection.ToString(),
        *NextComboName.ToString(),
        bManualPressReceived ? TEXT("MANUAL") : TEXT("AUTO-HOLD"));

    // Clear combo state
    NextComboName = NAME_None;
    bInComboWindow = false;
    bManualPressReceived = false;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ComboWindowTimer);
    }
}

void UGA_Attack::OnComboWindowExpired()
{
    UE_LOG(LogTemp, Log, TEXT("Combo window expired - chain will end"));

    NextComboName = NAME_None;
    bInComboWindow = false;
    bManualPressReceived = false;
}

AHWeapon* UGA_Attack::GetActiveWeapon() const
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
    if (!Player) { return nullptr; }

    UHInventoryComponent* Inventory = Player->GetInventoryComponent();
    if (!Inventory) { return nullptr; }

    return Inventory->GetActiveWeapon();
}

const FWeaponStatsRow* UGA_Attack::GetWeaponStats() const
{
    AHWeapon* Weapon = GetActiveWeapon();
    if (!Weapon) { return nullptr; }

    return Weapon->GetWeaponStats();
}

bool UGA_Attack::CanPayStaminaForAttack() const
{
    const FWeaponStatsRow* Stats = GetWeaponStats();
    if (!Stats)
    {
        return false;
    }

    if (!StaminaCostEffect)
    {
        return true; // No cost = can afford
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC)
    {
        return false;
    }

    // Get current stamina cost
    float StaminaCost = GetStaminaCostForCurrentSection();
    if (StaminaCost <= 0.0f)
    {
        return true; // No cost
    }

    // Check if we have enough stamina
    // This is a simplified check - you might want to use a proper cost GE spec
    return ASC->CanApplyAttributeModifiers(
        StaminaCostEffect.GetDefaultObject(),
        GetCurrentAbilityLevel(),
        ASC->MakeEffectContext()
    );
}

void UGA_Attack::ConsumeStamina()
{
    if (!StaminaCostEffect || !CachedWeaponStats)
    {
        return;
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC)
    {
        return;
    }

    // Create stamina cost effect spec
    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
        StaminaCostEffect,
        GetCurrentAbilityLevel()
    );

    if (!SpecHandle.IsValid())
    {
        return;
    }

    // Get cost for current section
    float StaminaCost = GetStaminaCostForCurrentSection();

    // Set the stamina cost via SetByCaller
    // Your StaminaCostEffect should have a SetByCaller modifier
    SpecHandle.Data->SetSetByCallerMagnitude(GetStaminaCostEventTag(), StaminaCost);

    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    ATTACK_LOG(Log, TEXT("Stamina consumed: %.1f"), StaminaCost);
}

float UGA_Attack::GetStaminaCostForCurrentSection() const
{
    if (!CachedWeaponStats || !CachedAnimInstance || !AttackMontage)
    {
        return CachedWeaponStats ? CachedWeaponStats->LightAttackStaminaCost : 0.0f;
    }

    const FName CurrentSection = CachedAnimInstance->Montage_GetCurrentSection(AttackMontage);
    
    if (const bool* bIsCharged = ChargedAttackSections.Find(CurrentSection))
    {
        return *bIsCharged 
            ? CachedWeaponStats->ChargedAttackStaminaCost 
            : CachedWeaponStats->LightAttackStaminaCost;
    }

    // Default to light attack if section not in map
    return CachedWeaponStats->LightAttackStaminaCost;
}

void UGA_Attack::DealDamage(const FGameplayAbilityTargetDataHandle& TargetData)
{
    if (!DamageEffect || !CachedWeaponStats)
    {
        return;
    }
    
    CurrentEventIgnoredActors.Empty(CurrentEventIgnoredActors.Num());
    
    CachedIgnoreActors.Reset();
    CachedIgnoreActors.Add(GetAvatarActorFromActorInfo());
    
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
    
    for (const FWeaponDamageModifier& Mod : CachedWeaponStats->DamageModifiers)
    {
        if (Mod.SetByCallerTag.IsValid())
        {
            SpecHandle.Data->SetSetByCallerMagnitude(
                Mod.SetByCallerTag,
                Mod.BaseDamage
            );
        }
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
        
        if (CurrentEventIgnoredActors.Contains(HitActor))
        {
            continue;
        }

        UAbilitySystemComponent* TargetASC = 
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

        if (!TargetASC)
        {
            continue;
        }

        SourceASC->ApplyGameplayEffectSpecToTarget(
            *SpecHandle.Data.Get(),
            TargetASC
        );

        ATTACK_LOG(Log, TEXT("✓ Damage applied to: %s"), *GetNameSafe(HitActor));
        
        CurrentEventIgnoredActors.Add(HitActor);

        if (HasAuthority(&CurrentActivationInfo) && CachedWeapon)
        {
            if (CachedWeaponStats->DurabilityCostPerHit > 0.0f)
            {
                CachedWeapon->ReduceDurability(CachedWeaponStats->DurabilityCostPerHit);
            }
            CachedWeapon->OnWeaponHit.Broadcast();
        }
    }
}

void UGA_Attack::ClearCachedData()
{
    CachedWeaponStats = nullptr;
    CachedWeapon = nullptr;
    CachedAnimInstance = nullptr;
    
}

FGameplayTag UGA_Attack::GetComboChangeEventTag() const
{
    return Tag_ComboChange;
}

FGameplayTag UGA_Attack::GetComboEndEventTag() const
{
    return Tag_ComboEnd;
}

FGameplayTag UGA_Attack::GetDamageEventTag() const
{
    return Tag_Damage; 
}

FGameplayTag UGA_Attack::GetStaminaEventTag() const
{
    return Tag_Stamina;
}

FGameplayTag UGA_Attack::GetStaminaCostEventTag() const
{
    return Tag_StaminaCost;
}

bool UGA_Attack::ShouldChainAttack() const
{
    if (!IsLocallyControlled())
    {
        return false;
    }

    if (!bIsInputHeld || !bAllowAutoCombo || bInComboWindow)
    {
        return false;
    }

    if (!CanPayStaminaForAttack())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot chain - not enough stamina"));
        return false;
    }

    return true;
}
