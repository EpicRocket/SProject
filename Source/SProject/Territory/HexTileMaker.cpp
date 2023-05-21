// Fill out your copyright notice in the Description page of Project Settings.


#include "HexTileMaker.h"


// Sets default values
AHexTileMaker::AHexTileMaker()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AHexTileMaker::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if(!TileBP)
	{
		return;
	}
	
	Tile2DArray.SetNumZeroed(GridHeight);
	for (int Row = 0; Row < GridHeight; Row++)
		Tile2DArray[Row].SetNumZeroed(GridWidth);

	const float SideOffset = 0.8660254f * SideLength;

	const TObjectPtr<UWorld> World = GetWorld();
	const FAttachmentTransformRules AttachRules = FAttachmentTransformRules::KeepRelativeTransform;
	for (int Row = 0; Row < GridHeight; Row++)
	{		
		for (int Column = 0; Column < GridWidth; Column++)
		{
			const TObjectPtr<ATerritoryTile> NewTile = World->SpawnActor<ATerritoryTile>(TileBP);
			NewTile->AttachToActor(this, AttachRules);

			const bool bOddColumn = (Column % 2) == 1;
			const float X = bOddColumn ? -2.0f * Row * SideOffset - SideOffset : (-2.0f * Row * SideOffset);
			const float Y = 1.5f * Column * SideLength;
			NewTile->SetActorRelativeLocation(FVector(X, Y, 0.0f));

			Tile2DArray[Row][Column] = NewTile;
		}
	}
}