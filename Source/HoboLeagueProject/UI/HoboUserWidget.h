// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HoboUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API UHoboUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* NewController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
	
private:
	
};
