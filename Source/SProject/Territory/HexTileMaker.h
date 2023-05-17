// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TerritoryTile.h"
#include "GameFramework/Actor.h"
#include "HexTileMaker.generated.h"

UCLASS()
class SPROJECT_API AHexTileMaker : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHexTileMaker();

	virtual void PostInitializeComponents() override;
	
protected:
	TArray<TArray<TObjectPtr<ATerritoryTile>>> Tile2DArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	TSubclassOf<ATerritoryTile> TileBP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	int32 GridWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	int32 GridHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	float SideLength;
};
