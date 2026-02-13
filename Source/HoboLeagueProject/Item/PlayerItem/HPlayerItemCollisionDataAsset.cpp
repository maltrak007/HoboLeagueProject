// Fill out your copyright notice in the Description page of Project Settings.


#include "HPlayerItemCollisionDataAsset.h"

void UHPlayerItemCollisionDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Clamp values
	TraceFrequency = FMath::Clamp(TraceFrequency, 10.0f, 120.0f);
	DebugDrawDuration = FMath::Clamp(DebugDrawDuration, 0.1f, 10.0f);

	// Validate sphere radii
	for (FItemTracePoint& Point : TracePoints)
	{
		Point.SphereRadius = FMath::Clamp(Point.SphereRadius, 1.0f, 100.0f);
	}
}

EDataValidationResult UHPlayerItemCollisionDataAsset::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = Super::IsDataValid(ValidationErrors);

	// Check for empty trace points
	if (TracePoints.Num() == 0)
	{
		ValidationErrors.Add(FText::FromString(TEXT("Weapon Collision Data has no trace points defined!")));
		Result = EDataValidationResult::Invalid;
	}

	// Check for duplicate socket names
	TSet<FName> UniqueSocketNames;
	for (const FItemTracePoint& Point : TracePoints)
	{
		if (Point.SocketName == NAME_None)
		{
			ValidationErrors.Add(FText::FromString(TEXT("Trace point has invalid socket name (None)")));
			Result = EDataValidationResult::Invalid;
		}

		if (UniqueSocketNames.Contains(Point.SocketName))
		{
			ValidationErrors.Add(FText::FromString(
				FString::Printf(TEXT("Duplicate socket name found: %s"), *Point.SocketName.ToString())
			));
			Result = EDataValidationResult::Invalid;
		}

		UniqueSocketNames.Add(Point.SocketName);
	}

	// Warn if trace frequency is very high
	if (TraceFrequency > 90.0f)
	{
		ValidationErrors.Add(FText::FromString(
			FString::Printf(TEXT("Warning: High trace frequency (%.1f) may impact performance"), TraceFrequency)
		));
		Result = EDataValidationResult::NotValidated; // Warning, not error
	}

	return Result;
}
