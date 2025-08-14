#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FHGameplayTags
{
public:
	static FHGameplayTags GameplayTags;
	
	static const FHGameplayTags& Get(){return GameplayTags;}
	
	static void InitializeNativeGameplayTags();
	
	FGameplayTag STATUS_BLEEDING;

	//**************************************************************
	//     ****************************************************
	//							ANIMATIONS
	//     ****************************************************
	//**************************************************************
	FGameplayTag Event_Combo_Change_Combo02;
	FGameplayTag Event_Combo_Change_End;

	//**************************************************************
	//     ****************************************************
	//							ABILITIES
	//     ****************************************************
	//**************************************************************
	FGameplayTag Abilities_ComboAttack;

	//**************************************************************
	//     ****************************************************
	//						  GAMEPLAY CUES
	//     ****************************************************
	//**************************************************************

	//**************************************************************
	//     ****************************************************
	//						 ACTIVATION FAIL
	//     ****************************************************
	//**************************************************************
	FGameplayTag Activation_Fail_BlockedByTags;
	FGameplayTag Activation_Fail_CantAffordCost;
	FGameplayTag Activation_Fail_IsDead;
	FGameplayTag Activation_Fail_MissingTags;
	FGameplayTag Activation_Fail_Networking;
	FGameplayTag Activation_Fail_OnCooldown;
};
