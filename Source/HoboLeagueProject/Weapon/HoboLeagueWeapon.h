// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoboLeagueWeapon.generated.h"

class USphereComponent;
class UWeaponDataAsset;

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
	
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* WeaponMesh;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY(Replicated)
	bool bIsPickedUp = false;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionSphere;
};
