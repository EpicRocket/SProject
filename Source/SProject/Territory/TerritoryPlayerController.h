// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TerritoryPlayerController.generated.h"

struct FInputActionValue;

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
	void SetConstructBuildingBP(const TSubclassOf<class ATerritoryBuilding>& InBuildingBP) { BuildingBP = InBuildingBP; }
	void SetMovedBuilding(TObjectPtr<class ATerritoryBuilding> InMovedBuilding) { MovedBuilding = InMovedBuilding; }
	void SetModeType(const ETerritoryModeType InModeType);


public:
	TSubclassOf<class ATerritoryBuilding> GetConstructBuildingBP() const { return BuildingBP; }
	TObjectPtr<class ATerritoryBuilding> GetMovedBuilding() const { return MovedBuilding; }
	const ETerritoryModeType& GetMode() const { return ModeType; }
	float GetDoubleClickTime() const { return DoubleClickTime; }

private:
	void OnTouchBegin(const FInputActionValue& ActionValue);
	void OnHold(const FInputActionValue& ActionValue);
	void OnTouchEnd(const FInputActionValue& ActionValue);
	void OnTap(const FInputActionValue& ActionValue);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Territory|Config", meta = (AllowPrivateAccess))
	class UInputMappingContext* InputMapping;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Territory|Config", meta = (AllowPrivateAccess))
	class UTerritoryInputConfigData* InputConfigData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Territory", meta = (AllowPrivateAccess))
	float DoubleClickTime;
	
	
private:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Territory|observation", meta = (AllowPrivateAccess))
	ETerritoryModeType ModeType;
	
	// For Construct Mode
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Territory|observation", meta = (AllowPrivateAccess))
	TSubclassOf<class ATerritoryBuilding> BuildingBP;

private:
	UPROPERTY()
	TObjectPtr<class ATerritoryBuilding> MovedBuilding;
};
