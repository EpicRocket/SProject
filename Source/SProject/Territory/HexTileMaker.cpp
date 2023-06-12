// Fill out your copyright notice in the Description page of Project Settings.


#include "HexTileMaker.h"

#include "SProject.h"


// Sets default values
AHexTileMaker::AHexTileMaker()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	bRunConstructionScriptOnDrag = false;
}

void AHexTileMaker::SpawnTile()
{
	if(!TileBP)
	{
		LOG_ERROR("Tile BP is not bound")
		return;
	}
	
	const float SideOffset = 0.8660254f * SideLength;
	
	const TObjectPtr<UWorld> World = GetWorld();
	const FAttachmentTransformRules AttachRules = FAttachmentTransformRules::KeepRelativeTransform;
	
	for (int Column = 0; Column < GridHeight; Column++)
	{		
		for (int Row = 0; Row < GridWidth; Row++)
		{
			const TObjectPtr<ATerritoryTile> NewTile = World->SpawnActor<ATerritoryTile>(TileBP);
			NewTile->AttachToActor(this, AttachRules);
	
			const bool bOddColumn = (Column % 2) == 1;
			const float X = bOddColumn ? -2.0f * Row * SideOffset - SideOffset : (-2.0f * Row * SideOffset);
			const float Y = 1.5f * Column * SideLength;
			NewTile->SetActorRelativeLocation(FVector(X, Y, 0.0f));
		}
	}
}
