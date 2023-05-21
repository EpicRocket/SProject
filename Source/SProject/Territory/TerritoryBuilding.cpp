// Fill out your copyright notice in the Description page of Project Settings.


#include "Territory/TerritoryBuilding.h"

#include "TerritoryDefine.h"
#include "TerritoryGameMode.h"
#include "TerritoryPlayerController.h"

// Sets default values
ATerritoryBuilding::ATerritoryBuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = SceneComp;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	
	DoubleClickTime = 0.0f;
}

// Called when the game starts or when spawned
void ATerritoryBuilding::BeginPlay()
{
	Super::BeginPlay();
	
	TerritoryGameMode = Cast<ATerritoryGameMode>(GetWorld()->GetAuthGameMode());
	DoubleClickTime = TerritoryGameMode->GetDoubleClickTime();
	ClickDelta = 0.0f;
}

// Called every frame
void ATerritoryBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ClickCount == 1)
	{
		ClickDelta += DeltaTime;
		if(ClickDelta < DoubleClickTime)
		{
			ClickDelta = 0.0f;
			ClickCount = 0;
		}
	}
}

void ATerritoryBuilding::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	
	if(ClickCount == 1)
	{
		OnDoubleClicked();
		ClickCount = 0;
	}
	else
	{
		++ClickCount;
	}
}

void ATerritoryBuilding::OnDoubleClicked()
{
	TerritoryPC->SetModeType(ETerritoryModeType::Move);
}
