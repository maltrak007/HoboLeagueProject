#include "FGameplayTags.h"
#include "GameplayTagsManager.h"

FHGameplayTags FHGameplayTags::GameplayTags;

void FHGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.STATUS_BLEEDING = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Bleeding"),
		FString("Bleeding Status Effect"));
}