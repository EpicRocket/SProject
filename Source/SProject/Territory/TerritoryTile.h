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
	TObjectPtr<USceneComponent> SceneComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile", meta = (AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile", meta = (AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> PreviewMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile", meta = (AllowPrivateAccess))
	bool bHasToEmpty;	

private:
	UPROPERTY()
	TObjectPtr<ATerritoryPlayerController> TerritoryPC;
	
	UPROPERTY()
	TObjectPtr<ATerritoryBuilding> OwnBuilding;

public:
	void ShowPreviewMesh(TObjectPtr<ATerritoryBuilding> Building, const FLinearColor& Color) const;

public:
	void SetBuilding(const TObjectPtr<ATerritoryBuilding> InBuilding) { OwnBuilding = InBuilding; }

public:
	FORCEINLINE TObjectPtr<ATerritoryBuilding> GetBuilding() const { return OwnBuilding; }

public:
	FORCEINLINE bool IsEmpty() const { return OwnBuilding == nullptr && !bHasToEmpty; }

private:
	bool bCursor;
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	virtual void NotifyActorOnReleased(FKey ButtonReleased) override;
};
