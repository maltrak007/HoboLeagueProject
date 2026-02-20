#pragma once

#include "CoreMinimal.h"
#include "HItemType.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon       UMETA(DisplayName = "Weapon"),
	Consumable	 UMETA(DisplayName = "Consumable"),
	Other        UMETA(DisplayName = "Other")
};

UENUM(BlueprintType)
enum class ERarityType : uint8
{
	Common      UMETA(DisplayName = "Common"),
	Rare        UMETA(DisplayName = "Rare"),
	Epic        UMETA(DisplayName = "Epic"),
	Legendary   UMETA(DisplayName = "Legendary")
};

//** USED TO ATTACH IT TO THE SKELETAL MESH IN THEIR CORRESPONDENT SOCKET **//
UENUM(BlueprintType)
enum class EItemSize : uint8
{
	ExtraSmall      UMETA(DisplayName = "ExtraSmall"),
	Small			UMETA(DisplayName = "Small"),
	Big             UMETA(DisplayName = "Big"),
	ExtraBig        UMETA(DisplayName = "ExtraBig")
};

UENUM(BlueprintType)
enum class EItemSkillScore : uint8
{
	Perfect      UMETA(DisplayName = "Perfect"),
	Good		 UMETA(DisplayName = "Good"),
	Bad          UMETA(DisplayName = "Bad"),
	Kiddo        UMETA(DisplayName = "Kiddo")
};