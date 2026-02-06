// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "HoboLeagueProject/Item/HBaseItem.h"
#include "HPlayerItem.generated.h"

class UHPlayerItemDataAsset;

UCLASS()
class HOBOLEAGUEPROJECT_API AHPlayerItem : public AHBaseItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHPlayerItem();

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UHPlayerItemDataAsset* ItemData;
	
	/** Attach the item to the player's mesh HOLSTER socket */
	UFUNCTION()
	void AttachToHolsterSocket(APlayerCharacter* Player);

	/** Attach the item to the player's mesh ACTIVE socket */
	UFUNCTION()
	void AttachToActiveSocket(APlayerCharacter* Player);
	
	/** Detach the item from the player and enable collision */
	UFUNCTION()
	void DetachFromPlayer();
	
	virtual void Interact_Implementation(APlayerCharacter* PlayerOwner) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
