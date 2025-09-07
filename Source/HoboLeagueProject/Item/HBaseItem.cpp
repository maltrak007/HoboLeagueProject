// Fill out your copyright notice in the Description page of Project Settings.


#include "HBaseItem.h"

#include "HBaseItemDataAsset.h"
#include "Components/SphereComponent.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInventoryComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHBaseItem::AHBaseItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(ItemMesh);
	CollisionSphere->SetSphereRadius(50.f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionSphere->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AHBaseItem::BeginPlay()
{
	Super::BeginPlay();
	//TODO::REPLACE THIS FOR THE SKELETAL OR THE STATIC MESH FROM THE DATA ASSET
	ItemMesh->SetStaticMesh(ItemData ? ItemData->GetItemMesh(): nullptr);
}

void AHBaseItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHBaseItem::OnItemOverlap);
}

void AHBaseItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CollisionSphere->OnComponentBeginOverlap.RemoveAll(this);
}

void AHBaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHBaseItem, bIsItemPickedUp);
	DOREPLIFETIME(AHBaseItem, OwningPlayer);
}

void AHBaseItem::ServerPickupItem_Implementation(APlayerCharacter* Player)
{
	if (!Player || bIsItemPickedUp) return;

	OwningPlayer = Player;
	bIsItemPickedUp = true;

	Player->InventoryComponent->AddItem(this);
	
	AttachToHolsterSocket(Player);
}

void AHBaseItem::AttachToHolsterSocket(APlayerCharacter* Player)
{
	if (!Player) return;
	
	//TODO::SUBSTITUE FOR THE INTERACTION COMPONENT
	// Disable collision so it can't be picked twice
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FString EnumAsNumber = FString::FromInt((uint8)ItemData->GetItemSize());
	// Attach to the player's mesh socket
	FString SocketNameStr = ItemData->GetItemName().ToString()+ TEXT("_") + EnumAsNumber + TEXT("_HolsterSocket");
	FName SocketName(*SocketNameStr);
	AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void AHBaseItem::DetachFromPlayer()
{
	// Detach actor from player
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetOwner(nullptr);
	SetActorEnableCollision(true);

	bIsItemPickedUp = false;
	OwningPlayer = nullptr;

	// Temporarily disable overlap to prevent instant pickup
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//TODO::SUBSTITUE FOR THE INTERACTION COMPONENT
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

void AHBaseItem::AttachToActiveSocket(APlayerCharacter* Player)
{
	if (!Player) return;
	//TODO::SUBSTITUE FOR THE INTERACTION COMPONENT
	// Disable collision so it can't be picked twice
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FString EnumAsNumber = FString::FromInt((uint8)ItemData->GetItemSize());
	// Attach to the player's mesh socket
	FString SocketNameStr = ItemData->GetItemName().ToString()+ TEXT("_") + EnumAsNumber + TEXT("_ActiveSocket");
	FName SocketName(*SocketNameStr);
	AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void AHBaseItem::OnRep_IsItemPickedUp()
{
	if (bIsItemPickedUp && OwningPlayer)
	{
		AttachToHolsterSocket(OwningPlayer);
	}
}

void AHBaseItem::OnItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player || bIsItemPickedUp) return;

	if (HasAuthority())
	{
		ServerPickupItem(Player);
	}
	else
	{
		ServerPickupItem(Player); // Call to server
	}
}