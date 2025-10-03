// Fill out your copyright notice in the Description page of Project Settings.


#include "HoboHUD.h"
#include "HoboUserWidget.h"
#include "OverlayWidgetController.h"

UOverlayWidgetController* AHoboHUD::GetOverlayWidgetController(const FWidgetControllerParams Params)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(Params);
		OverlayWidgetController->BindCallbacksToAttributes();
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

void AHoboHUD::InitOverlay(APlayerController* PC, APlayerState* PS,
	UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayHoboWidgetClass);
	OverlayHoboWidget = Cast <UHoboUserWidget>(Widget);
	
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* NewOverlayWidgetController = GetOverlayWidgetController(WidgetControllerParams);

	OverlayHoboWidget->SetWidgetController(NewOverlayWidgetController);

	NewOverlayWidgetController->BroadcastInitialValues();
	
	Widget->AddToViewport();
}
