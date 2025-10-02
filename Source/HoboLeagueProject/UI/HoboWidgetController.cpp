// Fill out your copyright notice in the Description page of Project Settings.


#include "HoboWidgetController.h"


FWidgetControllerParams::FWidgetControllerParams()
{
}

void UHoboWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& Params)
{
	Controller = Params.PlayerController;
	PlayerState = Params.PlayerState;
	AbilitySystemComponent = Params.AbilitySystemComponent;
	AttributeSet = Params.AttributeSet;
}
