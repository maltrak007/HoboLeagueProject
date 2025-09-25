#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FHGameplayTags
{
public:
	static FHGameplayTags GameplayTags;
	
	static const FHGameplayTags& Get(){return GameplayTags;}

	static FName GetTagName(const FGameplayTag& Tag)
	{
		return Tag.GetTagName();
	}
	
	static void InitializeNativeGameplayTags();
	
	//**************************************************************
	//     ****************************************************
	//							STATUS
	//     ****************************************************
	//**************************************************************
	FGameplayTag Status_Dead;
	FGameplayTag Status_Overdosing;
	FGameplayTag Status_StaminaDepletion;
	FGameplayTag Status_Bleeding;

	//**************************************************************
	//     ****************************************************
	//							ANIMATIONS
	//     ****************************************************
	//**************************************************************
	FGameplayTag Event_Combo_Change_PrimaryCombo02;
	FGameplayTag Event_Combo_Change_PrimaryCombo03;
	FGameplayTag Event_Combo_Change_PrimaryCombo04;
	FGameplayTag Event_Combo_Change_SecondaryCombo02;
	FGameplayTag Event_Combo_Change_SecondaryCombo03;
	FGameplayTag Event_Combo_Change_SecondaryCombo04;
	FGameplayTag Event_Combo_Change_Melee02;
	FGameplayTag Event_Combo_Change_Melee03;
	FGameplayTag Event_Combo_Change_Melee04;
	FGameplayTag Event_Combo_Change_End;
	FGameplayTag Event_Combo_Damage;

	//**************************************************************
	//     ****************************************************
	//						ACTIVE ABILITIES
	//     ****************************************************
	//**************************************************************
	FGameplayTag Abilities_PrimaryAttack;
	FGameplayTag Abilities_SecondaryAttack;
	FGameplayTag Abilities_MeleeAttack;
	FGameplayTag Abilities_UseConsumable;
	FGameplayTag Abilities_PickUp;
	FGameplayTag Abilities_DropDown;
	FGameplayTag Abilities_MSwapItem;
	FGameplayTag Abilities_WSwapItem;
	FGameplayTag Abilities_CSwapItem;

	//**************************************************************
	//     ****************************************************
	//						PASIVE ABILITIES
	//     ****************************************************
	//**************************************************************
	FGameplayTag Abilities_Interact;
	FGameplayTag Abilities_RegenerateStamina;
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
