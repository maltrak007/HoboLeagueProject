// Fill out your copyright notice in the Description page of Project Settings.

#include "AN_ItemCollisionTrace.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItem.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItemDataAsset.h"
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItemCollisionDataAsset.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshSocket.h"
#include "HoboLeagueProject/Item/PlayerItem/Weapon/HWeapon.h"

UAN_ItemCollisionTrace::UAN_ItemCollisionTrace()
{
	bIsNativeBranchingPoint = true;
	
	// Default event tag
	GameplayEventTag = FHGameplayTags::Get().Event_Combo_Damage;
}

void UAN_ItemCollisionTrace::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetWorld())
	{
		return;
	}

	// Initialize
	if (!Initialize(MeshComp))
	{
		UE_LOG(LogTemp, Warning, TEXT("AN_WeaponTargetTrace: Failed to initialize"));
		return;
	}

	// Reset state
	HitActors.Empty();
	PreviousSocketLocations.Empty();
	TraceTimeAccumulator = 0.0f;
	bIsActive = true;

	// ✅ FIXED: Store initial socket locations in WORLD SPACE
	if (CollisionData && CollisionData->bUseSweptTraces)
	{
		// ✅ Get the weapon's StaticMeshComponent (not the UStaticMesh asset!)
		if (UStaticMeshComponent* WeaponMeshComp = CachedWeapon->ItemMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Storing initial socket WORLD locations:"));
			
			for (const FItemTracePoint& TracePoint : CollisionData->TracePoints)
			{
				if (TracePoint.SocketName != NAME_None)
				{
					// ✅ Get socket location in WORLD SPACE
					FVector WorldLocation = WeaponMeshComp->GetSocketLocation(TracePoint.SocketName);
					PreviousSocketLocations.Add(TracePoint.SocketName, WorldLocation);
					
					UE_LOG(LogTemp, Warning, TEXT("  - Socket '%s': %s"), 
						*TracePoint.SocketName.ToString(),
						*WorldLocation.ToString());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Weapon has no StaticMeshComponent!"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("AN_WeaponTargetTrace: Begin - Weapon: %s, Trace Points: %d"),
		*GetNameSafe(CachedWeapon),
		CollisionData ? CollisionData->GetNumTracePoints() : 0);
}

void UAN_ItemCollisionTrace::NotifyTick(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!bIsActive || !MeshComp || !CollisionData)
	{
		return;
	}

	// Perform traces
	PerformTraces(MeshComp, FrameDeltaTime);
}

void UAN_ItemCollisionTrace::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Log, TEXT("AN_WeaponTargetTrace: End - Total unique hits: %d"), HitActors.Num());

	Cleanup();
}

FString UAN_ItemCollisionTrace::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Weapon Trace (%s)"), *GameplayEventTag.ToString());
}

// ============ Internal Functions ============

bool UAN_ItemCollisionTrace::Initialize(USkeletalMeshComponent* MeshComp)
{
	// Get owning actor
	AActor* Owner = GetOwningActor(MeshComp);
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("AN_WeaponTargetTrace: No owning actor"));
		return false;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	if (!ASC)	{
		UE_LOG(LogTemp, Warning, TEXT("AN_WeaponTargetTrace: Owner has no AbilitySystemComponent"));
		return false;
	}
	
	// Get active weapon
	CachedWeapon = GetActiveItem(Owner);
	if (!CachedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("AN_WeaponTargetTrace: No active weapon on %s"), *GetNameSafe(Owner));
		return false;
	}

	// Get collision data from weapon
	if (const UHPlayerItemDataAsset* ItemData = Cast<UHPlayerItemDataAsset>(CachedWeapon->ItemData))
	{
		CollisionData = ItemData->GetItemCollisionData();
	}

	if (!CollisionData)
	{
		UE_LOG(LogTemp, Warning, TEXT("AN_WeaponTargetTrace: Weapon '%s' has no collision data configured"),
			*GetNameSafe(CachedWeapon));
		return false;
	}

	if (!CollisionData->HasValidTracePoints())
	{
		UE_LOG(LogTemp, Warning, TEXT("AN_WeaponTargetTrace: Weapon collision data has no trace points"));
		return false;
	}

	return true;
}

void UAN_ItemCollisionTrace::PerformTraces(USkeletalMeshComponent* MeshComp, float DeltaTime)
{
	if (!CachedWeapon || !CollisionData)
	{
		return;
	}

	// Accumulate time for trace frequency control
	TraceTimeAccumulator += DeltaTime;

	const float TraceInterval = CollisionData->GetTraceInterval();
	if (TraceTimeAccumulator < TraceInterval)
	{
		return; // Not time to trace yet
	}

	TraceTimeAccumulator -= TraceInterval;

	// Get weapon mesh
	UStaticMeshComponent* WeaponMeshComp = CachedWeapon->ItemMesh;
	if (!WeaponMeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Weapon has no StaticMeshComponent!"));
		return;
	}

	TArray<FHitResult> FrameHits;

	// Perform trace for each point
	for (const FItemTracePoint& TracePoint : CollisionData->TracePoints)
	{
		if (TracePoint.SocketName == NAME_None)
		{
			continue;
		}

		// ✅ FIXED: Get socket location in WORLD SPACE
		FVector CurrentLocation = WeaponMeshComp->GetSocketLocation(TracePoint.SocketName);

		// Debug logging to verify world space
		if (bDebugDraw)
		{
			const float Distance = PreviousSocketLocations.Contains(TracePoint.SocketName) ?
				FVector::Dist(PreviousSocketLocations[TracePoint.SocketName], CurrentLocation) : 0.0f;
			
			UE_LOG(LogTemp, Warning, TEXT("Socket '%s' World Loc: %s (Distance from prev: %.2f cm)"), 
				*TracePoint.SocketName.ToString(),
				*CurrentLocation.ToString(),
				Distance);
		}

		// Perform swept or single trace
		if (CollisionData->bUseSweptTraces && PreviousSocketLocations.Contains(TracePoint.SocketName))
		{
			FVector PreviousLocation = PreviousSocketLocations[TracePoint.SocketName];
			PerformSweptTrace(MeshComp, TracePoint, PreviousLocation, CurrentLocation);
		}
		else
		{
			PerformSingleTrace(MeshComp, TracePoint, CurrentLocation);
		}

		// Update previous location (in world space)
		PreviousSocketLocations.Add(TracePoint.SocketName, CurrentLocation);
	}
}

void UAN_ItemCollisionTrace::PerformSingleTrace(
	USkeletalMeshComponent* MeshComp,
	const FItemTracePoint& TracePoint,
	const FVector& CurrentLocation)
{
	if (!MeshComp || !MeshComp->GetWorld())
	{
		return;
	}

	AActor* Owner = GetOwningActor(MeshComp);
	if (!Owner)
	{
		return;
	}

	// Setup trace parameters
	ECollisionChannel Channel = bOverrideTraceChannel ? TraceChannel : 
		(TracePoint.bUseCustomChannel ? TracePoint.CustomTraceChannel : CollisionData->DefaultTraceChannel);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Owner);
	if (CachedWeapon)
	{
		IgnoreActors.Add(CachedWeapon);
	}

	// Perform sphere trace at current location
	TArray<FHitResult> HitResults;
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		MeshComp->GetWorld(),
		CurrentLocation,
		CurrentLocation, // Start and end at same point (sphere overlap)
		TracePoint.SphereRadius,
		UEngineTypes::ConvertToTraceType(Channel),
		false, // bTraceComplex
		IgnoreActors,
		bDebugDraw ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults,
		true, // bIgnoreSelf
		FLinearColor::Red,
		FLinearColor::Green,
		2.0f // Debug duration
	);

	if (bHit)
	{
		ProcessHits(MeshComp, HitResults);
	}
}

void UAN_ItemCollisionTrace::PerformSweptTrace(
	USkeletalMeshComponent* MeshComp,
	const FItemTracePoint& TracePoint,
	const FVector& PreviousLocation,
	const FVector& CurrentLocation)
{
	if (!MeshComp || !MeshComp->GetWorld())
	{
		return;
	}

	AActor* Owner = GetOwningActor(MeshComp);
	if (!Owner)
	{
		return;
	}

	// Setup trace parameters
	ECollisionChannel Channel = bOverrideTraceChannel ? TraceChannel :
		(TracePoint.bUseCustomChannel ? TracePoint.CustomTraceChannel : CollisionData->DefaultTraceChannel);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Owner);
	if (CachedWeapon)
	{
		IgnoreActors.Add(CachedWeapon);
	}

	// Perform swept sphere trace
	TArray<FHitResult> HitResults;
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		MeshComp->GetWorld(),
		PreviousLocation,
		CurrentLocation,
		TracePoint.SphereRadius,
		UEngineTypes::ConvertToTraceType(Channel),
		false, // bTraceComplex
		IgnoreActors,
		bDebugDraw ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults,
		true, // bIgnoreSelf
		FLinearColor::Yellow,
		FLinearColor::Green,
		2.0f // Debug duration
	);

	if (bHit)
	{
		ProcessHits(MeshComp, HitResults);
	}
}

void UAN_ItemCollisionTrace::ProcessHits(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits)
{
	TArray<FHitResult> NewHits;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}

		// Skip if already hit this actor
		TWeakObjectPtr<AActor> WeakActor(HitActor);
		if (HitActors.Contains(WeakActor))
		{
			continue;
		}

		// Add to hit set
		HitActors.Add(WeakActor);
		NewHits.Add(Hit);

		UE_LOG(LogTemp, Log, TEXT("AN_WeaponTargetTrace: Hit '%s' at %s"),
			*GetNameSafe(HitActor),
			*Hit.Location.ToString());
	}

	// Send new hits to ability
	if (NewHits.Num() > 0)
	{
		SendHitEvent(MeshComp, NewHits);
	}
}

void UAN_ItemCollisionTrace::SendHitEvent(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits)
{
	AActor* Owner = GetOwningActor(MeshComp);
	if (!Owner)
	{
		return;
	}

	// Get ability system component
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("AN_WeaponTargetTrace: Owner has no AbilitySystemComponent"));
		return;
	}

	// Create target data from hits
	FGameplayAbilityTargetDataHandle TargetData;
	
	for (const FHitResult& Hit : Hits)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
		TargetData.Add(NewData);
	}

	// Create event data
	FGameplayEventData EventData;
	EventData.EventTag = GameplayEventTag;
	EventData.Instigator = Owner;
	EventData.Target = nullptr; // Multiple targets
	EventData.TargetData = TargetData;

	// Send event
	ASC->HandleGameplayEvent(GameplayEventTag, &EventData);

	UE_LOG(LogTemp, Log, TEXT("AN_WeaponTargetTrace: Sent event '%s' with %d hits"),
		*GameplayEventTag.ToString(),
		Hits.Num());
}

AActor* UAN_ItemCollisionTrace::GetOwningActor(USkeletalMeshComponent* MeshComp) const
{
	if (!MeshComp)
	{
		return nullptr;
	}

	// Try to get owner from mesh component
	AActor* Owner = MeshComp->GetOwner();
	
	// If mesh is on a weapon, get the weapon's owner (the character)
	if (!Owner)
	{
		// Try outer chain
		Owner = Cast<AActor>(MeshComp->GetOuter());
	}

	return Owner;
}

AHWeapon* UAN_ItemCollisionTrace::GetActiveItem(AActor* Owner) const
{
	if (!Owner)
	{
		return nullptr;
	}

	// Try to get from PlayerCharacter
	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(Owner))
	{
		if (UHInventoryComponent* Inventory = PlayerChar->GetInventoryComponent())
		{
			return Inventory->GetActiveWeapon();
		}
	}

	// Try to get weapon directly (if notify is on weapon mesh)
	if (AHWeapon* Item = Cast<AHWeapon>(Owner))
	{
		return Item;
	}

	return nullptr;
}

void UAN_ItemCollisionTrace::Cleanup()
{
	bIsActive = false;
	HitActors.Empty();
	PreviousSocketLocations.Empty();
	CachedWeapon = nullptr;
	CollisionData = nullptr;
	TraceTimeAccumulator = 0.0f;
}