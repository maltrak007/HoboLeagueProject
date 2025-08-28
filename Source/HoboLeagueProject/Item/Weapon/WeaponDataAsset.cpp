// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDataAsset.h"

#include "HoboLeagueProject/Item/HItemType.h"

UWeaponDataAsset::UWeaponDataAsset()
{
	ItemType = EItemType::Weapon;
	Damage = 25.f;
	Durability = 100.f;
}
