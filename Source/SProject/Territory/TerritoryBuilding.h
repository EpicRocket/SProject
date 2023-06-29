// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerritoryBuilding.generated.h"

UCLASS()
class SPROJECT_API ATerritoryBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerritoryBuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USceneComponent> SceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> MeshComp;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Territory")
	FLinearColor PreviewColorCan;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Territory")
	FLinearColor PreviewColorDeny;
	
private:
	TObjectPtr<class ATerritoryGameMode> TerritoryGameMode;
	TObjectPtr<class ATerritoryPlayerController> TerritoryPC;
	TObjectPtr<class ATerritoryTile> OwnerTile;
	
	float DoubleClickTime;
	float DoubleClickDelta;
	int32 ClickCount;
	bool bPressed;

public:
	void SetOwnerTile(TObjectPtr<class ATerritoryTile> Tile) { OwnerTile = Tile; }
	
public:
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetMeshComponent() const { return MeshComp; }
	FORCEINLINE TObjectPtr<class ATerritoryTile> GetOwnerTile() const { return OwnerTile; }

private:
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	virtual void NotifyActorOnReleased(FKey ButtonReleased) override;
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;
	void OnDoubleClicked();
};
