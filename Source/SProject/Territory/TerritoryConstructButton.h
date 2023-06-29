// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "TerritoryBuilding.h"
#include "TerritoryConstructButton.generated.h"

/**
 * 
 */
UCLASS()
class SPROJECT_API UTerritoryConstructButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString BuildingPath;
public:
	UFUNCTION(BlueprintSetter)
	void SetBuildingPath(FString InBuildingPath) { BuildingPath = InBuildingPath; }
	const FString& GetBuildingPath() const { return BuildingPath; }

private:
	virtual void NativeOnClicked() override;
};
