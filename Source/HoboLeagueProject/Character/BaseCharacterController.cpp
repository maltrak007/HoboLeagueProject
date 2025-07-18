// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"

ABaseCharacterController::ABaseCharacterController()
{
	bReplicates = true;
}

void ABaseCharacterController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalController())
	{
		// Delay initialization to wait for LocalPlayer to be fully available 
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseCharacterController::InitInputMapping);
	}
	UWorld* World = GetWorld();
	FName CurrentMapName = *World->GetMapName(); // Note: May include prefix like "UEDPIE_0_"
	CurrentMapName = FPackageName::GetShortFName(CurrentMapName); // Remove PIE or persistent level prefix

	if (CurrentMapName == "MultiplayerLobbyGym") // Replace with your actual map name
	{
		// Set to GameOnly input mode
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
	else
	{
		// Game + UI (e.g. in Lobby)
		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(false);
		SetInputMode(InputModeData);
		bShowMouseCursor = true;
	}
}

void ABaseCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacterController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacterController::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABaseCharacterController::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABaseCharacterController::Jump);
}

void ABaseCharacterController::InitInputMapping()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(DefaultPlayerInputMappingContext, 0);
			UE_LOG(LogTemp, Log, TEXT("Enhanced Input initialized successfully."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Subsystem is null in InitInputMapping"));
		}
	}
	else
	{
		// Retry on next tick if LocalPlayer is not yet ready
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseCharacterController::InitInputMapping);
	}
}

void ABaseCharacterController::Move(const FInputActionValue& Value)
{
	const FVector2D MovementValue = Value.Get<FVector2D>();
	if (APawn* ControlledPawn = GetPawn())
	{
		FRotator PlayerCharacterControlRotation = GetControlRotation();
		PlayerCharacterControlRotation.Pitch = 0.0f;
		PlayerCharacterControlRotation.Roll = 0.0f;

		const FVector ForwardDirection = FRotationMatrix(PlayerCharacterControlRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(PlayerCharacterControlRotation).GetUnitAxis(EAxis::Y);

		if (MovementValue.Y != 0.f)
		{
			ControlledPawn->AddMovementInput(ForwardDirection, MovementValue.Y);
		}
		if (MovementValue.X != 0.f)
		{
			ControlledPawn->AddMovementInput(RightDirection, MovementValue.X);
		}
	}
}

void ABaseCharacterController::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	AddYawInput(LookAxisValue.X);
	AddPitchInput(LookAxisValue.Y);
}

void ABaseCharacterController::Jump(const FInputActionValue& Value)
{
	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (Value.Get<bool>())
		{
			PlayerCharacter->Jump();
		}
		else
		{
			PlayerCharacter->StopJumping();
		}
	}
}
