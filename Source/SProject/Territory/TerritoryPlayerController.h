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
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;
	
public:
	const ETerritoryModeType& GetMode() const { return ModeType; }
	void SetModeType(const ETerritoryModeType InModeType);
	
	TSubclassOf<class ATerritoryBuilding> GetConstructBuildingBP() const { return BuildingBP; }
	void SetConstructBuildingBP(const TSubclassOf<class ATerritoryBuilding>& InBuildingBP) { BuildingBP = InBuildingBP; }
	void SetMovedBuilding(TObjectPtr<class ATerritoryBuilding> InMovedBuilding) { MovedBuilding = InMovedBuilding; }

public:
	TObjectPtr<class ATerritoryBuilding> GetMovedBuilding() const { return MovedBuilding; }
	float GetDoubleClickTime() const { return DoubleClickTime; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Territory Controll", meta = (AllowPrivateAccess))
	ETerritoryModeType ModeType;
	
	// For Construct Mode
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Territory Controll", meta = (AllowPrivateAccess))
	TSubclassOf<class ATerritoryBuilding> BuildingBP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory Controll", meta = (AllowPrivateAccess))
	float DoubleClickTime;

private:
	UPROPERTY()
	TObjectPtr<class ATerritoryBuilding> MovedBuilding;
};
