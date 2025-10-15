// Fill out your copyright notice in the Description page of Project Settings.


#include "HConsumableDataAsset.h"

#include "HoboLeagueProject/Item/HItemType.h"

UHConsumableDataAsset::UHConsumableDataAsset()
{
	ItemType = EItemType::Consumable;
	Charges = 1;
}
