// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "HBaseItem.generated.h"

class USphereComponent;
class UHBaseItemDataAsset;
class APlayerCharacter;

UCLASS()
class HOBOLEAGUEPROJECT_API AHBaseItem : public AActor, public IItemInteractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHBaseItem();
	
	/** Player that owns this item (replicated) */
	UPROPERTY(Replicated)
	APlayerCharacter* OwningPlayer;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	USkeletalMeshComponent* ItemSkeletalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	USphereComponent* CollisionSphere;

	UFUNCTION()
	void OnItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                   const FHitResult& SweepResult);

	UFUNCTION()
	void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
