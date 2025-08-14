#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

class UTexture2D;
class USkeletalMesh;
class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;
class UStaticMesh;

UCLASS(BlueprintType)
class HOBOLEAGUEPROJECT_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// ======================
	//   Weapon Stats
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float Durability;

	// ======================
	//   Weapon Effects
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Effects")
	FGameplayTagContainer StatusEffect;

	// ======================
	//   Visuals
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UTexture2D* WeaponIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UStaticMesh* WeaponMesh;

	// ======================
	//   Abilities
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> WeaponPrimaryAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> WeaponSecondaryAbility;

	// ======================
	//   Animation
	// ======================
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	// UAnimMontage* PrimaryAttackMontage;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	// UAnimMontage* SecondaryAttackMontage;

	// ======== GETTERS ========

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetDamage() const { return Damage; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetDurability() const { return Durability; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Effects")
	const FGameplayTagContainer& GetStatusEffect() const { return StatusEffect; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Visual")
	UTexture2D* GetWeaponIcon() const { return WeaponIcon; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Visual")
	UStaticMesh* GetWeaponMesh() const { return WeaponMesh; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Abilities")
	TSubclassOf<UGameplayAbility> GetWeaponPrimaryAbility() const { return WeaponPrimaryAbility; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Abilities")
	TSubclassOf<UGameplayAbility> GetWeaponSecondaryAbility() const { return WeaponSecondaryAbility; }
	
	// UFUNCTION(BlueprintPure, Category = "Weapon|Animation")
	// UAnimMontage* GetPrimaryAttackMontage() const { return PrimaryAttackMontage; }
	//
	// UFUNCTION(BlueprintPure, Category = "Weapon|Animation")
	// UAnimMontage* GetSecondaryAttackMontage() const { return SecondaryAttackMontage; }
};
