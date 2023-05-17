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
	TSubclassOf<ATerritoryBuilding> BuildingBP;
public:
	const TSubclassOf<ATerritoryBuilding>& GetBuilding() const { return BuildingBP; }

private:
	virtual void NativeOnClicked() override;
};
