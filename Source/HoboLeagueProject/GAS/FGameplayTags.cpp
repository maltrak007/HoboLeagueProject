#include "FGameplayTags.h"
#include "GameplayTagsManager.h"

FHGameplayTags FHGameplayTags::GameplayTags;

void FHGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.STATUS_BLEEDING = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Bleeding"),
		FString("Bleeding Status Effect"));

	//*****************************************************************************
	//      *****************************************************************
	//							Activation Fail Tags
	//      *****************************************************************
	//*****************************************************************************
	GameplayTags.Activation_Fail_BlockedByTags = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Activation.Fail.BlockedByTags"),
		FString("Activation Failed Blocked By Tags"));
	GameplayTags.Activation_Fail_CantAffordCost = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Activation.Fail.CantAffordCost"),
		FString("Activation Failed Can't Afford Cost"));
	GameplayTags.Activation_Fail_IsDead = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Activation.Fail.IsDead"),
		FString("Activation Failed Is Dead"));
	GameplayTags.Activation_Fail_MissingTags = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Activation.Fail.MissingTags"),
		FString("Activation Failed Missing Tags"));
	GameplayTags.Activation_Fail_Networking = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Activation.Fail.Networking"),
		FString("Activation Failed Networking"));
	GameplayTags.Activation_Fail_OnCooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Activation.Fail.OnCooldown"),
		FString("Activation Failed On Cooldown"));
}