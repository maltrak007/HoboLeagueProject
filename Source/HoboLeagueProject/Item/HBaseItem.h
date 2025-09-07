// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HBaseItem.generated.h"

class USphereComponent;
class UHBaseItemDataAsset;
class APlayerCharacter;

UCLASS()
class HOBOLEAGUEPROJECT_API AHBaseItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHBaseItem();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UHBaseItemDataAsset* ItemData;

	UPROPERTY(ReplicatedUsing = OnRep_IsItemPickedUp)
	bool bIsItemPickedUp = false;
	
	/** Player that owns this weapon (replicated) */
	UPROPERTY(Replicated)
	APlayerCharacter* OwningPlayer;
	
	UPROPERTY(VisibleAnywhere, Category = "Item")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionSphere;

	UFUNCTION()
	void OnItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Attach the item to the player's mesh HOLSTER socket */
	void AttachToHolsterSocket(APlayerCharacter* Player);

	/** Detach the item from the player and enable collision */
	void DetachFromPlayer();

	/** Put the item in the player current ACTIVE socket */
	void AttachToActiveSocket(APlayerCharacter* Player);

	/** Server RPC to handle pickup */
	UFUNCTION(Server, Reliable)
	void ServerPickupItem(APlayerCharacter* Player);
	
	/** Called when bIsPickedUp changes on clients */
	UFUNCTION()
	void OnRep_IsItemPickedUp();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
