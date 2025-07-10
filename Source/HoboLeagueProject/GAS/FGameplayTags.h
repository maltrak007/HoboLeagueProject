#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FHGameplayTags
{
public:
	static FHGameplayTags GameplayTags;
	
	static const FHGameplayTags& Get(){return GameplayTags;}
	
	static void InitializeNativeGameplayTags();

	// EXAMPLE Gameplay Tags
	//TODO:: INITIALIZE NATIVE GAMEPLAY TAGS IN DEFAULTENGINE.INI
	FGameplayTag STATUS_BLEEDING;
};
