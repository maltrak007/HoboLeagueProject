#include "FGameplayTags.h"
#include "GameplayTagsManager.h"

FHGameplayTags FHGameplayTags::GameplayTags;

void FHGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.STATUS_BLEEDING = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Bleeding"),
		FString("Bleeding Status Effect"));


	/*************************************************************************************/
	/*	 								ANIMATIONS										 */
	/*************************************************************************************/

	GameplayTags.Event_Combo_Change_Combo02 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.Combo02"),
		FString("Notify to launch second part of combo"));

	GameplayTags.Event_Combo_Change_End = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change.End"),
		FString("Notify end time for next part of the combo"));


	/*************************************************************************************/
	/*	 								ABILITIES										 */
	/*************************************************************************************/

	GameplayTags.Abilities_ComboAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.ComboAttack"),
	FString("Ability to ComboAttack"));

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
	FName("Activation_Fail_MissingTags"),
	FString("Activation Fail Missing Tags"));
	GameplayTags.Activation_Fail_Networking = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Activation.Fail.Networking"),
	FString("Activation Fail Networking"));
	GameplayTags.Activation_Fail_OnCooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Activation_Fail_OnCooldown"),
	FString("Activation Fail On Cooldown"));
	
}