// Fill out your copyright notice in the Description page of Project Settings.


#include "Territory/TerritoryTile.h"
#include "SProject.h"
#include "TerritoryDefine.h"
#include "TerritoryPlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"

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
	
	bHasToEmpty = false;
	bCursor = false;
}

void ATerritoryTile::BeginPlay()
{
	Super::BeginPlay();
	TerritoryPC = Cast<ATerritoryPlayerController>(GetWorld()->GetFirstPlayerController());
	VERIFY(TerritoryPC);
}

void ATerritoryTile::ShowPreviewMesh(TObjectPtr<ATerritoryBuilding> Building, const FLinearColor& Color) const
{
	PreviewMeshComp->SetVisibility(true);
	PreviewMeshComp->SetRelativeTransform(Building->GetMeshComponent()->GetRelativeTransform());
			
	const TObjectPtr<UStaticMesh> NewMesh = Building->GetMeshComponent()->GetStaticMesh();
	PreviewMeshComp->SetStaticMesh(NewMesh);
	for(int i = 0; i < PreviewMeshComp->GetNumMaterials(); ++i)
	{
		if(PreviewMeshComp->GetMaterial(i))
		{
			const auto MI = PreviewMeshComp->CreateAndSetMaterialInstanceDynamic(i);
			MI->SetVectorParameterValue(TEXT("Base Color Tint"), Color);
		}
	}
}

void ATerritoryTile::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	bCursor = true;

	VERIFY(TerritoryPC);
	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle:
		break;
	case ETerritoryModeType::Construct:
		{
			const TObjectPtr<ATerritoryBuilding> Building = TerritoryPC->GetConstructBuildingBP().GetDefaultObject();
			if(IsEmpty())
			{
				ShowPreviewMesh(Building, Building->PreviewColorCan);
			}
			else
			{
				ShowPreviewMesh(Building, Building->PreviewColorDeny);
			}
			break;
		}
	case ETerritoryModeType::Move:
		{
			const TObjectPtr<ATerritoryBuilding> Building = TerritoryPC->GetMovedBuilding();
			if(IsEmpty())
			{
				ShowPreviewMesh(Building, Building->PreviewColorCan);
			}
			else
			{
				ShowPreviewMesh(Building, Building->PreviewColorDeny);
			}
			break;
		}
	default:
		break;
	}
}

void ATerritoryTile::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	bCursor = false;

	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle:
		break;
	case ETerritoryModeType::Construct:
		PreviewMeshComp->SetStaticMesh(nullptr);
		break;
	case ETerritoryModeType::Move:
		PreviewMeshComp->SetStaticMesh(nullptr);
		break;
		
	default: ;
	}
}


void ATerritoryTile::NotifyActorOnClicked(FKey ButtonPressed)
{
	VERIFY(TerritoryPC);
	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle: break;
	case ETerritoryModeType::Construct: break;
	case ETerritoryModeType::Move: break;
	default: ;
	}
}

void ATerritoryTile::NotifyActorOnReleased(FKey ButtonReleased)
{
	Super::NotifyActorOnReleased(ButtonReleased);
	
	VERIFY(TerritoryPC);
	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle:
		break;

	case ETerritoryModeType::Construct:
		{
			if(IsEmpty())
			{
				const TObjectPtr<ATerritoryBuilding> TerritoryBuilding = TerritoryPC->GetConstructBuildingBP().GetDefaultObject();
				OwnBuilding = GetWorld()->SpawnActor<ATerritoryBuilding>(TerritoryPC->GetConstructBuildingBP());
				OwnBuilding->SetActorTransform(GetActorTransform());
				OwnBuilding->GetMeshComponent()->SetRelativeTransform(TerritoryBuilding->GetMeshComponent()->GetRelativeTransform());
				OwnBuilding->SetOwnerTile(this);
			
				PreviewMeshComp->SetStaticMesh(nullptr);
				PreviewMeshComp->SetVisibility(false);
				TerritoryPC->SetModeType(ETerritoryModeType::Idle);
			}
			break;
		}
		
	case ETerritoryModeType::Move:
		{
			const TObjectPtr<ATerritoryBuilding> MovedBuilding = TerritoryPC->GetMovedBuilding();
			const TObjectPtr<ATerritoryTile> MovedBuildingTile = MovedBuilding->GetOwnerTile();
			if(MovedBuildingTile != this)
			{
				if(IsEmpty())
				{
					PreviewMeshComp->SetVisibility(false);
					OwnBuilding = MovedBuildingTile->OwnBuilding;
					OwnBuilding->SetActorLocation(GetActorLocation());
					OwnBuilding->SetOwnerTile(this);
					MovedBuildingTile->SetBuilding(nullptr);
					TerritoryPC->SetModeType(ETerritoryModeType::Idle);
				}
				else
				{
					/*
					const TObjectPtr<ATerritoryBuilding> PrevBuilding = Building;
					Building = MovedBuilding;
					Building->SetActorLocation(GetActorLocation());
					
					PrevBuilding->SetOwnerTile(MovedBuildingTile);
					PrevBuilding->SetActorLocation(MovedBuilding->GetActorLocation());
					MovedBuildingTile->SetBuilding(PrevBuilding);
					*/
				}
			}
			break;
		}
	}
}
