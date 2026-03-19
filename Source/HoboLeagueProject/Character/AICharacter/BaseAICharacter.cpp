// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAICharacter.h"


ABaseAICharacter::ABaseAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


