// Fill out your copyright notice in the Description page of Project Settings.


#include "Territory/TerritoryTile.h"
#include "SProject.h"
#include "TerritoryDefine.h"
#include "TerritoryPlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"

static constexpr ECollisionChannel ECC_TerritoryTile = ECC_GameTraceChannel11;

// Sets default values
ATerritoryTile::ATerritoryTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneComp =  CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = SceneComp;
}

void ATerritoryTile::BeginPlay()
{
	Super::BeginPlay();
	TerritoryPC = Cast<ATerritoryPlayerController>(GetWorld()->GetFirstPlayerController());
	VERIFY(TerritoryPC)

	TArray<UMeshComponent*> MeshComponents;
	GetComponents(MeshComponents);
	for(const auto Mesh : MeshComponents)
	{		
		Mesh->SetCollisionResponseToChannel(ECC_TerritoryTile, ECollisionResponse::ECR_Block);
	}
	
	bHasToEmpty = false;
	bCursor = false;
}

void ATerritoryTile::ShowPreviewBuilding(const FLinearColor& Color) const
{
	const TObjectPtr<AActor> PreviewBuilding = TerritoryPC->GetPreviewBuilding();
	PreviewBuilding->SetActorTransform(GetActorTransform());
	
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	PreviewBuilding->GetComponents(StaticMeshComponents);
	for(const auto PreviewMesh : StaticMeshComponents)
	{
		for(int i = 0; i < PreviewMesh->GetNumMaterials(); ++i)
		{
			if(PreviewMesh->GetMaterial(i))
			{
				const auto MI = PreviewMesh->CreateAndSetMaterialInstanceDynamic(i);
				MI->SetVectorParameterValue(TEXT("Base Color Tint"), Color);
			}
		}
	}
	
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	PreviewBuilding->GetComponents(SkeletalMeshComponents);
	for(const auto PreviewMesh : SkeletalMeshComponents)
	{
		for(int i = 0; i < PreviewMesh->GetNumMaterials(); ++i)
		{
			if(PreviewMesh->GetMaterial(i))
			{
				const auto MI = PreviewMesh->CreateAndSetMaterialInstanceDynamic(i);
				MI->SetVectorParameterValue(TEXT("Base Color Tint"), Color);
			}
		}
	}
	TerritoryPC->SetPreviewBuildingVisiblity(true);
}

void ATerritoryTile::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	bCursor = true;

	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle:
		break;
	case ETerritoryModeType::Construct:
		{
			const TSubclassOf<ATerritoryBuilding> BuildingBP = TerritoryPC->GetConstructBuildingBP();
			const TObjectPtr<ATerritoryBuilding> Building = BuildingBP.GetDefaultObject();
			if(IsEmpty())
			{
				ShowPreviewBuilding(Building->PreviewColorCan);
			}
			else
			{
				ShowPreviewBuilding(Building->PreviewColorDeny);
			}
			break;
		}
	case ETerritoryModeType::Move:
		{
			const TObjectPtr<ATerritoryBuilding> Building = TerritoryPC->GetMovedBuilding();
			if(IsEmpty())
			{
				ShowPreviewBuilding(Building->PreviewColorCan);
			}
			else
			{
				ShowPreviewBuilding(Building->PreviewColorDeny);
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
		TerritoryPC->SetPreviewBuildingVisiblity(false);
		break;
	case ETerritoryModeType::Move:
		TerritoryPC->SetPreviewBuildingVisiblity(false);
		break;
		
	default: ;
	}
}


void ATerritoryTile::NotifyActorOnClicked(FKey ButtonPressed)
{
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
	
	switch(TerritoryPC->GetMode())
	{
	case ETerritoryModeType::Idle:
		break;

	case ETerritoryModeType::Construct:
		{
			if(IsEmpty())
			{
				const TSubclassOf<ATerritoryBuilding> BuildingBP = TerritoryPC->GetConstructBuildingBP();
				OwnBuilding = GetWorld()->SpawnActor<ATerritoryBuilding>(BuildingBP);
				OwnBuilding->SetActorTransform(GetActorTransform());
				OwnBuilding->SetOwnerTile(this);
				
				TerritoryPC->UnRegisterBuilding();
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
					OwnBuilding = MovedBuildingTile->OwnBuilding;
					OwnBuilding->SetActorLocation(GetActorLocation());
					OwnBuilding->SetOwnerTile(this);
					MovedBuildingTile->SetBuilding(nullptr);
					
					TerritoryPC->UnRegisterBuilding();
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
