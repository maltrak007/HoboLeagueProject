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
	
	// UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	// float GetDamage() const { return Damage; }
	
protected:
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	// float Damage;
};
