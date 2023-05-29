// Fill out your copyright notice in the Description page of Project Settings.


#include "Territory/TerritoryBuilding.h"

#include "TerritoryDefine.h"
#include "TerritoryGameMode.h"
#include "TerritoryPlayerController.h"

// Sets default values
ATerritoryBuilding::ATerritoryBuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = SceneComp;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATerritoryBuilding::BeginPlay()
{
	Super::BeginPlay();
	TerritoryPC = Cast<ATerritoryPlayerController>(GetWorld()->GetFirstPlayerController());
	TerritoryGameMode = Cast<ATerritoryGameMode>(GetWorld()->GetAuthGameMode());
	DoubleClickTime = TerritoryPC->GetDoubleClickTime();
	DoubleClickDelta = 0.0f;
	bPressed = false;
}

// Called every frame
void ATerritoryBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//더블 클릭 여부
	if(ClickCount == 1)
	{
		DoubleClickDelta += DeltaTime;
		if(DoubleClickDelta > DoubleClickTime)
		{
			DoubleClickDelta = 0.0f;
			ClickCount = 0;
		}
	}
}

void ATerritoryBuilding::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	ClickCount++;
	if(ClickCount == 1)
	{
		bPressed = true;
		TerritoryPC->SetModeType(ETerritoryModeType::Move);
		TerritoryPC->SetMovedBuilding(this);
	}
	else if(ClickCount == 2)
	{
		OnDoubleClicked();
	}
}

void ATerritoryBuilding::OnDoubleClicked()
{
	DoubleClickDelta = 0.0f;
	ClickCount = 0;
}

void ATerritoryBuilding::NotifyActorOnReleased(FKey ButtonReleased)
{
	Super::NotifyActorOnReleased(ButtonReleased);

	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle: break;
	case ETerritoryModeType::Construct: break;
	case ETerritoryModeType::Move: break;
	}
}

void ATerritoryBuilding::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
}

void ATerritoryBuilding::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
}

