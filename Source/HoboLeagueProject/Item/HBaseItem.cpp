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
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHBaseItem::OnItemOverlap);
}

// Called when the game starts or when spawned
void AHBaseItem::BeginPlay()
{
	Super::BeginPlay();
	ItemMesh->SetStaticMesh(ItemData ? ItemData->GetItemMesh(): nullptr);
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
	
	AttachToPlayer(Player);
}

void AHBaseItem::AttachToPlayer(APlayerCharacter* Player)
{
	if (!Player) return;

	// Disable collision so it can't be picked twice
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Attach to the player's mesh socket
	FString SocketNameStr = ItemData->GetItemName().ToString() + TEXT("Socket");
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

void AHBaseItem::PutInHolster()
{
	
}

void AHBaseItem::OnRep_IsItemPickedUp()
{
	if (bIsItemPickedUp && OwningPlayer)
	{
		AttachToPlayer(OwningPlayer);
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