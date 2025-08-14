#include "HoboLeagueWeapon.h"
#include "Components/SphereComponent.h"
#include "HoboLeagueProject/Character/Player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

AHoboLeagueWeapon::AHoboLeagueWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = WeaponMesh;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(WeaponMesh);
	CollisionSphere->SetSphereRadius(50.f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionSphere->SetGenerateOverlapEvents(true);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHoboLeagueWeapon::OnWeaponOverlap);
}

void AHoboLeagueWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AHoboLeagueWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHoboLeagueWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player || bIsPickedUp) return;

	if (HasAuthority())
	{
		ServerPickupWeapon(Player);
	}
	else
	{
		ServerPickupWeapon(Player); // Call to server
	}
}

void AHoboLeagueWeapon::ServerPickupWeapon_Implementation(APlayerCharacter* Player)
{
	if (!Player || bIsPickedUp) return;

	OwningPlayer = Player;
	bIsPickedUp = true;

	Player->Server_EquipWeapon(this);
	
	AttachToPlayer(Player);
}

void AHoboLeagueWeapon::AttachToPlayer(APlayerCharacter* Player)
{
	if (!Player) return;

	// Disable collision so it can't be picked twice
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Attach to the player's mesh socket
	AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
}

void AHoboLeagueWeapon::OnRep_IsPickedUp()
{
	if (bIsPickedUp && OwningPlayer)
	{
		AttachToPlayer(OwningPlayer);
	}
}

void AHoboLeagueWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHoboLeagueWeapon, bIsPickedUp);
	DOREPLIFETIME(AHoboLeagueWeapon, OwningPlayer);
}
