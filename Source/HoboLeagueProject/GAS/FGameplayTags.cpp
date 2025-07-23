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
	
}