// Fill out your copyright notice in the Description page of Project Settings.


#include "HPlayerItem.h"

#include "Components/SphereComponent.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"


// Sets default values
AHPlayerItem::AHPlayerItem()
{
}

// Called when the game starts or when spawned
void AHPlayerItem::BeginPlay()
{
	Super::BeginPlay();
}

void AHPlayerItem::AttachToHolsterSocket(APlayerCharacter* Player)
{
	if (!Player) return;

	// Disable collision so it can't be picked twice
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FString EnumAsNumber = FString::FromInt(static_cast<uint8>(ItemData->GetItemSize()));

	// Attach to the player's mesh socket
	FString SocketNameStr = ItemData->GetItemName().ToString() + TEXT("_") + EnumAsNumber + TEXT("_HolsterSocket");
	FName SocketName(*SocketNameStr);
	AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void AHPlayerItem::AttachToActiveSocket(APlayerCharacter* Player)
{
	if (!Player) return;

	// Disable collision so it can't be picked twice || This is done here because the player can autoequip items from the ground
	// so the item goes firstly to the holster socket if not done by the options menu and then to the active socket if autoequip is enabled
	if (CollisionSphere->GetCollisionEnabled() != ECollisionEnabled::NoCollision)
	{
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	FString EnumAsNumber = FString::FromInt(static_cast<uint8>(ItemData->GetItemSize()));
	// Attach to the player's mesh socket
	FString SocketNameStr = ItemData->GetItemName().ToString() + TEXT("_") + EnumAsNumber + TEXT("_ActiveSocket");
	FName SocketName(*SocketNameStr);
	AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void AHPlayerItem::DetachFromPlayer()
{
	// Detach actor from player
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetOwner(nullptr);
	SetActorEnableCollision(true);

	OwningPlayer = nullptr;

	// Temporarily disable overlap to prevent instant pickup
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Restore collision after short delay
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (IsValid(this))
		{
			CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}, 0.5f, false);
}

void AHPlayerItem::Interact_Implementation(APlayerCharacter* PlayerOwner)
{
	if (PlayerOwner)
	{
		OwningPlayer = PlayerOwner;

		OwningPlayer->InventoryComponent->AddItem(this);

		AttachToHolsterSocket(OwningPlayer);
	}
}
