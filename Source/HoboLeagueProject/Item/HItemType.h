#pragma once

#include "CoreMinimal.h"
#include "HItemType.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon       UMETA(DisplayName = "Weapon"),
	Consumable	 UMETA(DisplayName = "Consumable"),
	Melee		 UMETA(DisplayName = "Melee"),
	Other        UMETA(DisplayName = "Other")
};
