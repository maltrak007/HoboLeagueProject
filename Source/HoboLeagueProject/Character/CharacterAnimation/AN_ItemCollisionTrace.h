// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HoboLeagueProject/GAS/FGameplayTags.h"
#include "AN_ItemCollisionTrace.generated.h"

class AHWeapon;
class AHPlayerItem;
class UHPlayerItemCollisionDataAsset;
struct FItemTracePoint;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UAN_ItemCollisionTrace : public UAnimNotifyState
{
	GENERATED_BODY()

	UAN_ItemCollisionTrace();

	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float FrameDeltaTime,
		const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

protected:
	// ============ Configuration ============

	// Gameplay Event tag to send hit data to ability
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Trace")
	FGameplayTag GameplayEventTag;

	// Override trace channel (if not using weapon's default)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Trace")
	bool bOverrideTraceChannel = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Trace", meta=(EditCondition="bOverrideTraceChannel"))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	// Draw debug traces
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Trace|Debug")
	bool bDebugDraw = false;

	// ============ Runtime State ============

	// Cached weapon reference
	UPROPERTY()
	TObjectPtr<AHPlayerItem> CachedWeapon;

	// Cached collision data
	UPROPERTY()
	TObjectPtr<const UHPlayerItemCollisionDataAsset> CollisionData;

	// Previous frame socket locations (for swept traces)
	TMap<FName, FVector> PreviousSocketLocations;

	// Actors hit this notify (prevent duplicate hits)
	TSet<TWeakObjectPtr<AActor>> HitActors;

	// Time accumulator for trace frequency
	float TraceTimeAccumulator = 0.0f;

	// Is notify currently active?
	bool bIsActive = false;

	// ============ Internal Functions ============

	// Initialize notify state
	bool Initialize(USkeletalMeshComponent* MeshComp);

	// Perform traces for current frame
	void PerformTraces(USkeletalMeshComponent* MeshComp, float DeltaTime);

	// Perform a single trace between two points
	void PerformSingleTrace(
		USkeletalMeshComponent* MeshComp,
		const FItemTracePoint& TracePoint,
		const FVector& CurrentLocation);

	// Perform a swept trace between previous and current location
	void PerformSweptTrace(
		USkeletalMeshComponent* MeshComp,
		const FItemTracePoint& TracePoint,
		const FVector& PreviousLocation,
		const FVector& CurrentLocation);

	// Process hit results and filter duplicates
	void ProcessHits(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits);

	// Send hit results to ability via gameplay event
	void SendHitEvent(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits);

	// Get owning character
	AActor* GetOwningActor(USkeletalMeshComponent* MeshComp) const;

	// Get item from character
	AHWeapon* GetActiveItem(AActor* Owner) const;

	// Cleanup
	void Cleanup();
};
