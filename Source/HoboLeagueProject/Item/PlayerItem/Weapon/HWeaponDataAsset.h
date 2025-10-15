#pragma once

#include "CoreMinimal.h"
#include <Engine/DataAsset.h>
#include "HoboLeagueProject/Item/PlayerItem/HPlayerItemDataAsset.h"
#include "HWeaponDataAsset.generated.h"



UCLASS(BlueprintType)
class HOBOLEAGUEPROJECT_API UHWeaponDataAsset : public UHPlayerItemDataAsset
{
	GENERATED_BODY()

public:
	UHWeaponDataAsset();
	
	// ======================
	//   Weapon Stats
	// ======================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float Damage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float TotalDurability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float LostDurability;
	
	// ======== GETTERS ========
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetDamage() const { return Damage; }
	
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetTotalDurability() const { return TotalDurability; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetLostDurability() const { return LostDurability; }
};
