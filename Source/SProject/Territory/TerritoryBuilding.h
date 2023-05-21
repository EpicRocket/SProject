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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> SceneComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> MeshComp;

private:
	TObjectPtr<class ATerritoryGameMode> TerritoryGameMode;
	TObjectPtr<class ATerritoryPlayerController> TerritoryPC;
	
	float DoubleClickTime;
	float ClickDelta;
	int32 ClickCount;
	
public:
	TObjectPtr<UStaticMeshComponent> GetMeshComponent() const { return MeshComp; }

private:
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	void OnDoubleClicked();
};
