// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TerritoryPlayerController.generated.h"

enum class ETerritoryModeType : uint8;

/**
 * 
 */
UCLASS()
class SPROJECT_API ATerritoryPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATerritoryPlayerController();
	
public:
	const ETerritoryModeType& GetMode() const { return ModeType; }
	void SetModeType(const ETerritoryModeType InModeType);
	
	TSubclassOf<class ATerritoryBuilding> GetConstructBuildingBP() const { return BuildingBP; }
	void SetConstructBuildingBP(const TSubclassOf<class ATerritoryBuilding>& InBuildingBP) { BuildingBP = InBuildingBP; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	ETerritoryModeType ModeType;
	
	// For Construct Mode
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TSubclassOf<class ATerritoryBuilding> BuildingBP;
	
};
