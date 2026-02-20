// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "HoboLeagueProject/Item/HBaseItem.h"
#include "HoboLeagueProject/Item/HItemType.h"
#include "HPlayerItem.generated.h"

enum class ERarityType: uint8;
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
	TObjectPtr<UHPlayerItemDataAsset> ItemData;
	
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
	
	UFUNCTION(BlueprintCallable)
	void ReduceDurability(float AmountToReduce);

	UFUNCTION(BlueprintCallable)
	void RestoreDurability(float AmountToRestore);
	
	UPROPERTY(BlueprintReadOnly, Category = "Item|Instance", Replicated)
	float ItemDurability = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Instance", 
		ReplicatedUsing=OnRep_ItemRarity)
	ERarityType ItemRarity = ERarityType::Common;
	
	virtual const void* GetStatsForRarity() const { return nullptr; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnRep_ItemRarity();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
