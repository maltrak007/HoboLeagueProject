// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"

void UAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp->GetOwner())
	{
		return;
	}

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!OwnerASC)
	{
		return;
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(),EventTag,FGameplayEventData());
	
}

FString UAN_SendGameplayEvent::GetNotifyName_Implementation() const
{
	Super::GetNotifyName_Implementation();
	
	if (EventTag.IsValid())
	{
	/*
		FString TagString = EventTag.ToString(); // Ej: "Gameplay.Ability.Fire"
		TArray<FString> Parts;
		TagString.ParseIntoArray(Parts, TEXT("."), true);

		if (Parts.Num() > 0)
		{
			return Parts.Last();
		}
		*/
		
		
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag,TagNames);
		return TagNames.Last().ToString();
		
	}
	return "None";
}
