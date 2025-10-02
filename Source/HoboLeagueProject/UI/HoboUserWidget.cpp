// Fill out your copyright notice in the Description page of Project Settings.


#include "HoboUserWidget.h"

void UHoboUserWidget::SetWidgetController(UObject* NewController)
{
	WidgetController = NewController;
	WidgetControllerSet();
}
