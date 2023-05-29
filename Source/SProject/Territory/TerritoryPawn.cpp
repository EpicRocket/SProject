// Fill out your copyright notice in the Description page of Project Settings.


#include "Territory/TerritoryPawn.h"

// Sets default values
ATerritoryPawn::ATerritoryPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATerritoryPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATerritoryPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATerritoryPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

