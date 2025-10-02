// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HoboHUD.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
struct FWidgetControllerParams;
class UOverlayWidgetController;
class UHoboUserWidget;
/**
 * 
 */
UCLASS()
class HOBOLEAGUEPROJECT_API AHoboHUD : public AHUD
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TObjectPtr<UHoboUserWidget> OverlayHoboWidget;

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams Params);

	void InitOverlay(APlayerController* PC, APlayerState* PlayerState, UAbilitySystemComponent* AbilitySystemComponent, UAttributeSet* AttributeSet);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UHoboUserWidget> OverlayHoboWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
};
