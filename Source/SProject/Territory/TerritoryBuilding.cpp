// Fill out your copyright notice in the Description page of Project Settings.


#include "Territory/TerritoryBuilding.h"

// Sets default values
ATerritoryBuilding::ATerritoryBuilding()
{
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = SceneComp;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATerritoryBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATerritoryBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
