// Fill out your copyright notice in the Description page of Project Settings.


#include "HBaseItem.h"
#include "HBaseItemDataAsset.h"
#include "Components/SphereComponent.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "HoboLeagueProject/Component/HInteractionComponent.h"
#include "Net/UnrealNetwork.h"

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
	//CollisionSphere->SetCollisionProfileName(TEXT("ItemCollision"));
	CollisionSphere->SetGenerateOverlapEvents(true);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHBaseItem::OnItemOverlap);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AHBaseItem::OnItemEndOverlap);
}

void AHBaseItem::BeginPlay()
{
	Super::BeginPlay();
	ItemMesh->SetStaticMesh(ItemData ? ItemData->GetItemMesh() : nullptr);
}

void AHBaseItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CollisionSphere->OnComponentBeginOverlap.RemoveAll(this);
	CollisionSphere->OnComponentEndOverlap.RemoveAll(this);
}

void AHBaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHBaseItem, OwningPlayer);
}

void AHBaseItem::OnItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player) return;
	
	Player->InteractionComponent->AddInteractableObject(this);
}

void AHBaseItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player) return;

	Player->InteractionComponent->RemoveInteractableObject(this);
}



