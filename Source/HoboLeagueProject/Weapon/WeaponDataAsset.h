// Fill out your copyright notice in the Description page of Project Settings.

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
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	// Daño base del arma
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float Damage;

	// Durabilidad del arma
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float Durability;

	// Efectos de estado que aplica el arma
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Effects")
	FGameplayTagContainer StatusEffect;

	// Ícono del arma para UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UTexture2D* WeaponIcon;

	// Malla visual del arma
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	// USkeletalMesh* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UStaticMesh* WeaponMesh;
	
	// Habilidad primaria (ataque principal)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> WeaponPrimaryAbility;

	// Habilidad secundaria (ataque alterno, cargado, etc.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> WeaponSecondaryAbility;

	// GameplayEffect que se aplica al jugador al equipar el arma
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	// TSubclassOf<UGameplayEffect> WeaponGE;

	// Animación del ataque primario
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* PrimaryAttackMontage;

	// Animación del ataque secundario
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SecondaryAttackMontage;
protected:

private:
	
};
