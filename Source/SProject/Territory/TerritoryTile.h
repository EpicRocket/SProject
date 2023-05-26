// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TerritoryBuilding.h"
#include "GameFramework/Actor.h"
#include "TerritoryTile.generated.h"

UCLASS()
class SPROJECT_API ATerritoryTile : public AActor
{
	GENERATED_BODY()
	
public:
	ATerritoryTile();

	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile", meta = (AllowPrivateAccess))
	USceneComponent* SceneComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile", meta = (AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile", meta = (AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> PreviewMeshComp;

	UPROPERTY()
	TObjectPtr<ATerritoryBuilding> Building;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile", meta = (AllowPrivateAccess))
	bool bIsEmpty;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile", meta = (AllowPrivateAccess))
	int a;

public:
	FORCEINLINE bool IsEmpty() const { return bIsEmpty; }

private:
	bool bCursor;
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
};
