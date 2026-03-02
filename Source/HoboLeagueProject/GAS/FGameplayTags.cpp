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

	GameplayTags.Status_Overdosing = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Overdosing"),
		FString("Overdosing Status Effect"));

	GameplayTags.Status_StaminaDepletion = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.StaminaDepletion"),
		FString("Stamina Depletion Status Effect"));

	GameplayTags.Status_Bleeding = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Bleeding"),
		FString("Bleeding Status Effect"));

	/*************************************************************************************/
	/*	 								ANIMATIONS										 */
	/*************************************************************************************/
	GameplayTags.Event_Combo_Change = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Change"),
		FString("Notify to change combo"));
	
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

	GameplayTags.Event_Combo_UseStamina = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.UseStamina"),
		FString("Notify to Use Stamina"));

	GameplayTags.Event_Consume = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Consume"),
		FString("Notify to consume item"));

	GameplayTags.Event_PlayMinigame = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.PlayMinigame"),
		FString("Notify to Play Minigame"));
	
	/*************************************************************************************/
	/*	 								ABILITIES										 */
	/*************************************************************************************/

	GameplayTags.Abilities_PrimaryAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.PrimaryAttack"),
		FString("Ability to Primary Attack"));
	
	GameplayTags.Abilities_PrimaryHoldAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.PrimaryHoldAttack"),
		FString("Ability to Primary Hold Attack"));
	
	GameplayTags.Abilities_SecondaryAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.SecondaryAttack"),
		FString("Ability to Secondary Attack"));

	GameplayTags.Abilities_SecondaryHoldAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.SecondaryHoldAttack"),
		FString("Ability to Secondary Hold Attack"));
	
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
	/*	 						     PASIVE ABILITIES									 */
	/*************************************************************************************/
	GameplayTags.Abilities_Interact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Interact"),
		FString("Ability to Interact"));
	GameplayTags.Abilities_RegenerateStamina = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.RegenerateStamina"),
		FString("Regenerate Stamina Ability"));

	/*************************************************************************************/
	/*	 						     ITEM EQUIPPED   									 */
	/*************************************************************************************/
	GameplayTags.Item_Weapon_Fist = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Item.Weapon.Fist"),
			FString("Fist Weapon"));
	GameplayTags.Item_Weapon_Knife = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Weapon.Knife"),
		FString("Knife Weapon"));
	GameplayTags.Item_Weapon_Sword = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Weapon.Sword"),
		FString("Sword Weapon"));
	GameplayTags.Item_Weapon_Crowbar = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Weapon.Crowbar"),
		FString("Crowbar Weapon"));
	GameplayTags.Item_Weapon_BaseballBat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Weapon.BaseballBat"),
		FString("BaseballBat Weapon"));
	GameplayTags.Item_Weapon_StopSign = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Weapon.StopSign"),
		FString("StopSign Weapon"));

	GameplayTags.Item_Consumable_Drinkable_Pill = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Consumable.Drinkable.Pill"),
		FString("Pill Consumable"));
	GameplayTags.Item_Consumable_Cigarette = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Consumable.Cigarette"),
		FString("Cigarette Consumable"));
	GameplayTags.Item_Consumable_Drinkable_Beer = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Consumable.Drinkable.Beer"),
		FString("Beer Consumable"));
	GameplayTags.Item_Consumable_Glue = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Consumable.Glue"),
		FString("Glue Consumable"));
	GameplayTags.Item_Consumable_FentanylSyringe = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Consumable.FentanylSyringe"),
		FString("FentanylSyringe Consumable"));
	GameplayTags.Item_Consumable_AdrenalineSyringe = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Consumable.AdrenalineSyringe"),
		FString("AdrenalineSyringe Consumable"));

	/*************************************************************************************/
	/*	 						    OVERDRIVE TAGS   									 */
	/*************************************************************************************/
	GameplayTags.Overdrive_Consumable_Pill = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Consumable.Pill"),
		FString("Pill Overdrive"));
	GameplayTags.Overdrive_Consumable_Cigarette = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Consumable.Cigarette"),
		FString("Cigarette Overdrive"));
	GameplayTags.Overdrive_Consumable_Beer = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Consumable.Beer"),
		FString("Beer Overdrive"));
	GameplayTags.Overdrive_Consumable_Glue = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Consumable.Glue"),
		FString("Glue Overdrive"));
	GameplayTags.Overdrive_Consumable_FentanylSyringe = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Consumable.FentanylSyringe"),
		FString("FentanylSyringe Overdrive"));
	GameplayTags.Overdrive_Consumable_AdrenalineSyringe = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Consumable.AdrenalineSyringe"),
		FString("AdrenalineSyringe Overdrive"));

	GameplayTags.Overdrive_Status_GlassBones = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Status.GlassBones"),
		FString("GlassBones Overdrive Status"));
	GameplayTags.Overdrive_Status_NeedlePhobia = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Status.NeedlePhobia"),
		FString("NeedlePhobia Overdrive Status"));
	GameplayTags.Overdrive_Status_JabberingJaw = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Status.JabberingJaw"),
		FString("JabberingJaw Overdrive Status"));

	GameplayTags.Overdrive_Status_SuperJunkie = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Status.SuperJunkie"),
		FString("SuperJunkie Overdrive Status"));
	GameplayTags.Overdrive_Status_MrCharlieSheen = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Status.MrCharlieSheen"),
		FString("MrCharlieSheen Overdrive Status"));
	GameplayTags.Overdrive_Status_PimPumTakeLacasitos = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Overdrive.Status.PimPumTakeLacasitos"),
		FString("PimPumTakeLacasitos Overdrive Status"));
	
	/*************************************************************************************/
	/*	 								DATA TAGS									     */
	/*************************************************************************************/
	GameplayTags.Data_Sprint_Speed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.Sprint.Speed"),
		FString("Sprint Data Tag"));

	GameplayTags.Data_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.Damage"),
		FString("Damage Data Tag"));

	GameplayTags.Data_Overdose = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.Overdose"),
		FString("Overdose Data Tag"));

	GameplayTags.Data_Stamina = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Data.Stamina"),
	FString("Stamina Data Tag"));
	
	GameplayTags.Data_StaminaCost = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.StaminaCost"),
		FString("StaminaCost Data Tag"));
	
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
