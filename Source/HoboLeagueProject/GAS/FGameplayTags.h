#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FGameplayTags
{
public:
	static FGameplayTags GameplayTags;
	
	static const FGameplayTags& Get(){return GameplayTags;}
	
	static void InitializeNativeGameplayTags();
};
