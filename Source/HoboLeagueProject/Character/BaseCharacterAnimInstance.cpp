// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"

#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		CharacterMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwningCharacter)
	{
		Speed = OwningCharacter->GetVelocity().Length();
	}
	if(CharacterMovementComponent)
	{
		bIsJumping = CharacterMovementComponent->IsFalling();
	}
	UpdateMovementDirection();
}

void UBaseCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UBaseCharacterAnimInstance::UpdateMovementDirection()
{
	if (!OwningCharacter) return;

	FVector Velocity = OwningCharacter->GetVelocity();
	Velocity.Z = 0; // Ignore vertical movement

	if (Velocity.IsNearlyZero())
	{
		MovementDirection = 0.f;
		return;
	}

	FRotator ActorRotation = OwningCharacter->GetActorRotation();
	FVector Forward = ActorRotation.Vector(); // Forward vector
	FVector Right = FRotationMatrix(ActorRotation).GetUnitAxis(EAxis::Y);

	FVector NormalizedVelocity = Velocity.GetSafeNormal();

	float ForwardDot = FVector::DotProduct(Forward, NormalizedVelocity);
	float RightDot = FVector::DotProduct(Right, NormalizedVelocity);

	MovementDirection = FMath::Atan2(RightDot, ForwardDot); // In radians
	MovementDirection = FMath::RadiansToDegrees(MovementDirection); // Convert to degrees
}