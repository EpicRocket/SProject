// Fill out your copyright notice in the Description page of Project Settings.


#include "Territory/TerritoryTile.h"
#include "SProject.h"
#include "TerritoryDefine.h"
#include "TerritoryPlayerController.h"

// Sets default values
ATerritoryTile::ATerritoryTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneComp =  CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = SceneComp;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	MeshComp->SetupAttachment(RootComponent);
	
	PreviewMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMeshComp"));
	PreviewMeshComp->SetupAttachment(RootComponent);
	
	bIsEmpty = true;
	bCursor = false;
}

void ATerritoryTile::BeginPlay()
{
	Super::BeginPlay();
}

void ATerritoryTile::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	bCursor = true;

	const TObjectPtr<ATerritoryPlayerController> TerritoryPC = Cast<ATerritoryPlayerController>(GetWorld()->GetFirstPlayerController());

	VERIFY(TerritoryPC);
	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle:
		break;
	case ETerritoryModeType::Construct:
		if(IsEmpty())
		{
			CALLINFO
			const TObjectPtr<ATerritoryBuilding> TerritoryBuilding = TerritoryPC->GetConstructBuildingBP().GetDefaultObject();
			PreviewMeshComp->SetRelativeTransform(TerritoryBuilding->GetMeshComponent()->GetRelativeTransform());
			
			const TObjectPtr<UStaticMesh> NewMesh = TerritoryBuilding->GetMeshComponent()->GetStaticMesh();
			PreviewMeshComp->SetStaticMesh(NewMesh);
		}
		break;
	}
}

void ATerritoryTile::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	bCursor = false;
	
	const TObjectPtr<ATerritoryPlayerController> TerritoryPC = Cast<ATerritoryPlayerController>(GetWorld()->GetFirstPlayerController());

	VERIFY(TerritoryPC);
	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle:
		break;
	case ETerritoryModeType::Construct:
		CALLINFO
		PreviewMeshComp->SetStaticMesh(nullptr);
		break;
	}
}


void ATerritoryTile::NotifyActorOnClicked(FKey ButtonPressed)
{
	const TObjectPtr<ATerritoryPlayerController> TerritoryPC = Cast<ATerritoryPlayerController>(GetWorld()->GetFirstPlayerController());

	VERIFY(TerritoryPC);
	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle:
		break;
	case ETerritoryModeType::Construct:
		if(IsEmpty())
		{
			const TObjectPtr<ATerritoryBuilding> TerritoryBuilding = TerritoryPC->GetConstructBuildingBP().GetDefaultObject();
			Building = GetWorld()->SpawnActor<ATerritoryBuilding>(TerritoryPC->GetConstructBuildingBP());
			Building->SetActorTransform(GetActorTransform());
			Building->GetMeshComponent()->SetRelativeTransform(TerritoryBuilding->GetMeshComponent()->GetRelativeTransform());
			
			PreviewMeshComp->SetStaticMesh(nullptr);
			TerritoryPC->SetModeType(ETerritoryModeType::Idle);
			bIsEmpty = false;
		}
		break;
	}
}