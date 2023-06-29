// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "TerritoryPlayerController.generated.h"

struct FInputActionValue;

enum class ETerritoryModeType : uint8;

/**
 * 
 */
UCLASS()
class SPROJECT_API ATerritoryPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:
	ATerritoryPlayerController();
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
public:
	void RegisterBuilding(TSubclassOf<class ATerritoryBuilding> InBuildingBP);
	void UnRegisterBuilding();
	void SetPreviewBuildingVisiblity(bool bEnabled);
	void SetMovedBuilding(TObjectPtr<class ATerritoryBuilding> InMovedBuilding) { MovedBuilding = InMovedBuilding; }
	void SetModeType(const ETerritoryModeType InModeType);


public:
	TSubclassOf<class ATerritoryBuilding> GetConstructBuildingBP() const { return BuildingBP; }
	TObjectPtr<class ATerritoryBuilding> GetMovedBuilding() const { return MovedBuilding; }
	const ETerritoryModeType& GetMode() const { return ModeType; }
	float GetDoubleClickTime() const { return DoubleClickTime; }
	TObjectPtr<AActor> GetPreviewBuilding() const { return PreviewBuilding; }

private:
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchBegin(const FInputActionValue& ActionValue);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnHold(const FInputActionValue& ActionValue);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchEnd(const FInputActionValue& ActionValue);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTap(const FInputActionValue& ActionValue);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnPan(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void StartPan();
	FVector StartPanPos;

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
	UPROPERTY()
	TObjectPtr<AActor> PreviewBuilding;

private:
	UPROPERTY()
	TObjectPtr<class ATerritoryBuilding> MovedBuilding;
};
