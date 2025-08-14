// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueWeapon.generated.h"

class USphereComponent;
class UWeaponDataAsset;
class APlayerCharacter;

UCLASS()
class HOBOLEAGUEPROJECT_API AHoboLeagueWeapon : public AActor
{
	GENERATED_BODY()

public:
	AHoboLeagueWeapon();
	
	virtual void Tick(float DeltaTime) override;

	// Asegura que el overlap se maneje sólo en el servidor
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UWeaponDataAsset* WeaponData;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when bIsPickedUp changes on clients */
	UFUNCTION()
	void OnRep_IsPickedUp();

	/** Actually attaches weapon to the player’s mesh */
	void AttachToPlayer(APlayerCharacter* Player);

	/** Server RPC to handle pickup */
	UFUNCTION(Server, Reliable)
	void ServerPickupWeapon(APlayerCharacter* Player);
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_IsPickedUp)
	bool bIsPickedUp = false;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* WeaponMesh;

	/** Player that owns this weapon (replicated) */
	UPROPERTY(Replicated)
	APlayerCharacter* OwningPlayer;
};
