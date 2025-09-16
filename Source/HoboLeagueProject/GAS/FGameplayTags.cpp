#include "FGameplayTags.h"
#include "GameplayTagsManager.h"

FHGameplayTags FHGameplayTags::GameplayTags;

void FHGameplayTags::InitializeNativeGameplayTags()
{
	/*************************************************************************************/
	/*	 									STATUS  									 */
	/*************************************************************************************/
	GameplayTags.Status_Dead = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Dead"),
		FString("Dead Status Effect"));

	GameplayTags.Status_Bleeding = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Bleeding"),
		FString("Bleeding Status Effect"));


	/*************************************************************************************/
	/*	 								ANIMATIONS										 */
	/*************************************************************************************/

	GameplayTags.Event_Combo_Change_PrimaryCombo02 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.PrimaryCombo02"),
		FString("Notify to launch second primary part of combo"));

	GameplayTags.Event_Combo_Change_PrimaryCombo03 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.PrimaryCombo03"),
		FString("Notify to launch third primary part of combo"));

	GameplayTags.Event_Combo_Change_PrimaryCombo04 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.PrimaryCombo04"),
		FString("Notify to launch fourth primary part of combo"));

	GameplayTags.Event_Combo_Change_SecondaryCombo02 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.SecondaryCombo02"),
		FString("Notify to launch second secondary part of combo"));

	GameplayTags.Event_Combo_Change_SecondaryCombo03 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.SecondaryCombo03"),
		FString("Notify to launch third secondary part of combo"));

	GameplayTags.Event_Combo_Change_SecondaryCombo04 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.SecondaryCombo04"),
		FString("Notify to launch fourth secondary part of combo"));

	GameplayTags.Event_Combo_Change_Melee02 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.Melee02"),
		FString("Notify to launch secondary melee part of combo"));

	GameplayTags.Event_Combo_Change_Melee03 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.Melee03"),
		FString("Notify to launch third melee part of combo"));

	GameplayTags.Event_Combo_Change_Melee04 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.Melee04"),
		FString("Notify to launch fourth melee part of combo"));

	GameplayTags.Event_Combo_Change_End = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.End"),
		FString("Notify end time for next part of the combo"));

	GameplayTags.Event_Combo_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Damage"),
		FString("Notify to apply damage to target"));


	/*************************************************************************************/
	/*	 								ABILITIES										 */
	/*************************************************************************************/

	GameplayTags.Abilities_PrimaryAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.PrimaryAttack"),
	FString("Ability to Primary Attack"));

	GameplayTags.Abilities_SecondaryAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Abilities.SecondaryAttack"),
FString("Ability to Secondary Attack"));

	GameplayTags.Abilities_MeleeAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Abilities.MeleeAttack"),
FString("Ability to Melee Attack"));

	GameplayTags.Abilities_UseConsumable = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Abilities.UseConsumable"),
FString("Ability to Use Consumable"));

	GameplayTags.Abilities_PickUp = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Abilities.PickUp"),
FString("Ability to PickUp"));

	GameplayTags.Abilities_DropDown = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Abilities.DropDown"),
FString("Ability to DropDown"));

	GameplayTags.Abilities_MSwapItem = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Abilities.MSwapItem"),
FString("Ability to swap to Melee"));

	GameplayTags.Abilities_WSwapItem = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Abilities.WSwapItem"),
FString("Ability to swap to Weapon"));

	GameplayTags.Abilities_CSwapItem = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Abilities.CSwapItem"),
FString("Ability to swap to Consumable"));

	/*************************************************************************************/
	/*	 							ACTIVATION FAIL 									 */
	/*************************************************************************************/
	GameplayTags.Activation_Fail_BlockedByTags = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Activation.Fail.BlockedByTags"),
	FString("Activation Fail Blocked By Tags"));
	GameplayTags.Activation_Fail_CantAffordCost = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Activation.Fail.CantAffordCost"),
	FString("Activation Fail Cant Afford Cost"));
	GameplayTags.Activation_Fail_IsDead = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Activation.Fail.IsDead"),
	FString("Activation Fail Is Dead"));
	GameplayTags.Activation_Fail_MissingTags = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Activation.Fail.MissingTags"),
	FString("Activation Fail Missing Tags"));
	GameplayTags.Activation_Fail_Networking = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Activation.Fail.Networking"),
	FString("Activation Fail Networking"));
	GameplayTags.Activation_Fail_OnCooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Activation.Fail.OnCooldown"),
	FString("Activation Fail On Cooldown"));
	
}