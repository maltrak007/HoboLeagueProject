// Fill out your copyright notice in the Description page of Project Settings.


#include "HPlayerItem.h"

#include "HPlayerItemDataAsset.h"
#include "Components/SphereComponent.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInteractionComponent.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "HoboLeagueProject/Item/HBaseItemDataAsset.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AHPlayerItem::AHPlayerItem() : ItemData(nullptr)
{
	CollisionSphere->SetCollisionProfileName(TEXT("PlayerItemCollision"));
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// Called when the game starts or when spawned
void AHPlayerItem::BeginPlay()
{
	Super::BeginPlay();
	ItemMesh->SetStaticMesh(ItemData ? ItemData->GetItemMesh() : nullptr);
	//ItemDurability = ItemData ? ItemData->GetTotalDurability() : 0.f;
}

void AHPlayerItem::OnRep_ItemRarity()
{
	UE_LOG(LogTemp, Log, TEXT("Item '%s' rarity updated to: %s"),
		*GetName(),
		*UEnum::GetValueAsString(ItemRarity));
}

void AHPlayerItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHPlayerItem, ItemRarity);
	DOREPLIFETIME(AHPlayerItem, ItemDurability);
}


void AHPlayerItem::AttachToHolsterSocket(APlayerCharacter* Player)
{
	if (!Player) return;

	// Disable collision so it can't be picked twice
	CollisionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	FString EnumAsNumber = FString::FromInt(static_cast<uint8>(ItemData->GetItemSize()));

	// Attach to the player's mesh socket
	FString SocketNameStr = ItemData->ItemID.ToString() + TEXT("_") + EnumAsNumber + TEXT("_HolsterSocket");
	FName SocketName(*SocketNameStr);
	AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void AHPlayerItem::AttachToActiveSocket(APlayerCharacter* Player)
{
	if (!Player) return;

	// Disable collision so it can't be picked twice || This is done here because the player can autoequip items from the ground
	// so the item goes firstly to the holster socket if not done by the options menu and then to the active socket if autoequip is enabled

	CollisionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	FString EnumAsNumber = FString::FromInt(static_cast<uint8>(ItemData->GetItemSize()));
	// Attach to the player's mesh socket
	FString SocketNameStr = ItemData->ItemID.ToString() + TEXT("_") + EnumAsNumber + TEXT("_ActiveSocket");
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

	// Restore collision after short delay
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (IsValid(this))
		{
			CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
			CollisionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
	}, 0.5f, false);
}

void AHPlayerItem::Interact_Implementation(APlayerCharacter* PlayerOwner)
{
	if (PlayerOwner)
	{
		OwningPlayer = PlayerOwner;

		OwningPlayer->GetInventoryComponent()->AddItem(this);

		OwningPlayer->GetInteractionComponent()->RemoveInteractableObject(this);
	}
}

void AHPlayerItem::ReduceDurability(float AmountToReduce)
{
	if (!ItemData) return;
	
	ItemDurability = FMath::Max(0.0f, ItemDurability - AmountToReduce);
    
	if (ItemDurability <= 0.0f)
	{
		// Item broken
		UE_LOG(LogTemp, Log, TEXT("Item '%s' durability depleted!"), *GetName());
		// TODO: Trigger broken event, remove abilities, etc.
	}
	
	if (ItemDurability <= 0)
	{
		if (UHInventoryComponent* InvComp = OwningPlayer->GetInventoryComponent())
		{
			//TODO:: ""REMOVE AND MOVE IT TO ANOTHER PLACE TO REUSE" "
			RestoreDurability(100.f);
			InvComp->RemoveItem(this);
		}
	}
}

void AHPlayerItem::RestoreDurability(float AmountToRestore)
{
	//CHANGE IT TO SEARCH IN THE TABLE
	ItemDurability = FMath::Clamp(
		ItemDurability + AmountToRestore,
		0,
		100
	);
}
