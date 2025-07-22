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

	//*****************************************************************************
	//      *****************************************************************
	//							Activation Fail Tags
	//      *****************************************************************
	//*****************************************************************************
	FGameplayTag Activation_Fail_BlockedByTags;
	FGameplayTag Activation_Fail_CantAffordCost;
	FGameplayTag Activation_Fail_IsDead;
	FGameplayTag Activation_Fail_MissingTags;
	FGameplayTag Activation_Fail_Networking;
	FGameplayTag Activation_Fail_OnCooldown;
};
