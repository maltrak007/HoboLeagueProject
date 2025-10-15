// Fill out your copyright notice in the Description page of Project Settings.


#include "HWeaponDataAsset.h"

#include "HoboLeagueProject/Item/HItemType.h"

UHWeaponDataAsset::UHWeaponDataAsset()
{
	ItemType = EItemType::Weapon;
	Damage = 25.f;
	TotalDurability = 100.f;
}
