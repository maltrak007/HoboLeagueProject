// Fill out your copyright notice in the Description page of Project Settings.


#include "HoboLeagueWeapon.h"

#include "Components/SphereComponent.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AHoboLeagueWeapon::AHoboLeagueWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//AActor::SetReplicateMovement(true);
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = WeaponMesh;
	
	// Create and attach the sphere component
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(WeaponMesh);
	CollisionSphere->SetSphereRadius(50.f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionSphere->SetGenerateOverlapEvents(true);

	// Bind the overlap event
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHoboLeagueWeapon::OnWeaponOverlap);
}

// Called when the game starts or when spawned
void AHoboLeagueWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHoboLeagueWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player || bIsPickedUp) return;

	if (HasAuthority())
	{
		Player->EquipWeapon(this);
	}
	else
	{
		// Si es cliente, pedir al servidor que lo haga
		Player->Server_EquipWeapon(this);
	}
}

// Called every frame
void AHoboLeagueWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHoboLeagueWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHoboLeagueWeapon, bIsPickedUp);
}

