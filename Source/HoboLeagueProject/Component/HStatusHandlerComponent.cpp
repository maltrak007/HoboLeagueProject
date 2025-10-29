// Fill out your copyright notice in the Description page of Project Settings.


#include "HStatusHandlerComponent.h"
#include "GameFramework/PlayerState.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "HoboLeagueProject/GAS/HAbilitySystemComponent.h"
#include "HoboLeagueProject/GAS/GameplayAbility/Ability/PlayerAbility/GA_RegenerateStamina.h"


// Sets default values for this component's properties
UHStatusHandlerComponent::UHStatusHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UHStatusHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHStatusHandlerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AbilitySystemComp)
	{
		AbilitySystemComp->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_Dead)),
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

		AbilitySystemComp->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_StaminaDepletion)),
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

		AbilitySystemComp->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_Overdosing)),
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UHStatusHandlerComponent::LinkAbilitySystemComponent(UAbilitySystemComponent* ASC)
{
	AbilitySystemComp = ASC;

	//I Choose this approach because this method automatically replicates the changes to all clients
	// and it is more efficient than using a multicast RPC
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_Dead)),
			EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &UHStatusHandlerComponent::OnDeathTagChanged);

		ASC->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_StaminaDepletion)),
			EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &UHStatusHandlerComponent::OnStaminaDepletionTagChanged);

		ASC->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(
				FHGameplayTags::GetTagName(FHGameplayTags::Get().Status_Overdosing)),
			EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &UHStatusHandlerComponent::OnOverdoseTagChanged);
	}
}


// ---------- Death / Respawn ---------- //
void UHStatusHandlerComponent::OnDeathTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		HandlePlayerDeath();
	}
	else
	{
		HandlePlayerRespawn();
	}
}

void UHStatusHandlerComponent::HandlePlayerDeath() const
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
		PawnOwner->DisableInput(PC);
		PawnOwner->SetActorEnableCollision(false);
		PawnOwner->SetActorHiddenInGame(true);
	}
}

void UHStatusHandlerComponent::HandlePlayerRespawn() const
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
		PawnOwner->EnableInput(PC);
		PawnOwner->SetActorEnableCollision(true);
		PawnOwner->SetActorHiddenInGame(false);
	}
}

void UHStatusHandlerComponent::OnStaminaDepletionTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (!AbilitySystemComp || !GetOwner()) return;

	if (NewCount > 0)
	{
		FGameplayAbilitySpec* Spec = AbilitySystemComp->FindAbilitySpecFromClass(UGA_RegenerateStamina::StaticClass());
		if (Spec && Spec->Ability)
		{
			bool bActiveRegen = AbilitySystemComp->TryActivateAbility(Spec->Handle, true);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Stamina Regeneration Ability Activated: %s"), bActiveRegen ? TEXT("True") : TEXT("False")));
		}
	}
	else
	{
		FGameplayAbilitySpec* Spec = AbilitySystemComp->FindAbilitySpecFromClass(UGA_RegenerateStamina::StaticClass());
		if (Spec && Spec->Ability)
		{
			AbilitySystemComp->CancelAbility(Spec->Ability);
		}
	}
}

// ---------- Overdose ---------- //
void UHStatusHandlerComponent::OnOverdoseTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		HandlePlayerOverdose();
	}
}

void UHStatusHandlerComponent::HandlePlayerOverdose()
{
	// START A TIMER TO KILL THE PLAYER AFTER A FEW SECONDS
	// SHOW WIDGET TO INDICATE THE TIME LEFT TO DIE AND THE KEY TO ACTIVATE THE VOICE CHAT TO CALL FOR HELP
	// IF ANOTHER PLAYER HELPS HIM, CLEAR THE TIMER AND REMOVE THE WIDGET IF NOT KILL THE PLAYER
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PLAYER IS OVERDOSING"));
}

// ----------------------------------------- //
