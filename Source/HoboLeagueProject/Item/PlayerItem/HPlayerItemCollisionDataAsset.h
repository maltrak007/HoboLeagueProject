// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HPlayerItemCollisionDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FItemTracePoint
{
	GENERATED_BODY()

	// Socket name on the weapon mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FName SocketName = NAME_None;

	// Sphere radius for this trace point (in cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta=(ClampMin="1.0", ClampMax="100.0"))
	float SphereRadius = 10.0f;

	// Optional: Custom collision channel for this specific point
	// If not set, uses the weapon's default channel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bUseCustomChannel = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta=(EditCondition="bUseCustomChannel"))
	TEnumAsByte<ECollisionChannel> CustomTraceChannel = ECC_Pawn;

	// Constructor
	FItemTracePoint()
		: SocketName(NAME_None)
		, SphereRadius(10.0f)
		, bUseCustomChannel(false)
		, CustomTraceChannel(ECC_Pawn)
	{
	}

	FItemTracePoint(FName InSocketName, float InRadius = 10.0f)
		: SocketName(InSocketName)
		, SphereRadius(InRadius)
		, bUseCustomChannel(false)
		, CustomTraceChannel(ECC_Pawn)
	{
	}

	// Equality operator for TSet/TMap usage
	bool operator==(const FItemTracePoint& Other) const
	{
		return SocketName == Other.SocketName;
	}

	// Hash function for TSet/TMap usage
	friend uint32 GetTypeHash(const FItemTracePoint& TracePoint)
	{
		return GetTypeHash(TracePoint.SocketName);
	}
};
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHPlayerItemCollisionDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	// Array of trace points defining weapon collision
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
	TArray<FItemTracePoint> TracePoints;

	// Default trace channel for all points (unless point has custom channel)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
	TEnumAsByte<ECollisionChannel> DefaultTraceChannel = ECC_Pawn;

	// How often to perform traces (times per second)
	// Higher = more accurate but more expensive
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision|Performance", meta=(ClampMin="10.0", ClampMax="120.0"))
	float TraceFrequency = 60.0f;

	// Whether to perform traces between consecutive points (swept traces)
	// True = More accurate, False = Faster
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision|Accuracy")
	bool bUseSweptTraces = true;

	// Draw debug visualization
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision|Debug")
	bool bDebugDraw = false;

	// Debug draw duration (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision|Debug", meta=(EditCondition="bDebugDraw"))
	float DebugDrawDuration = 2.0f;

	// Validation
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#if WITH_EDITOR
	// Validate trace points
	EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif

	// Helper functions
	UFUNCTION(BlueprintPure, Category = "Collision")
	int32 GetNumTracePoints() const { return TracePoints.Num(); }

	UFUNCTION(BlueprintPure, Category = "Collision")
	bool HasValidTracePoints() const { return TracePoints.Num() > 0; }

	UFUNCTION(BlueprintPure, Category = "Collision")
	float GetTraceInterval() const { return 1.0f / FMath::Max(TraceFrequency, 1.0f); }
};
