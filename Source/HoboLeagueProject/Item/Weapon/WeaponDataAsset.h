#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"
#include "WeaponDataAsset.generated.h"



UCLASS(BlueprintType)
class HOBOLEAGUEPROJECT_API UWeaponDataAsset : public UHBaseItemDataAsset
{
	GENERATED_BODY()

public:
	// ======================
	//   Item Stats
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float Damage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float Durability;

	// ======== GETTERS ========

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetDamage() const { return Damage; }
	
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetDurability() const { return Durability; }
};
