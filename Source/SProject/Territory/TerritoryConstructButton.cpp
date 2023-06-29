// Fill out your copyright notice in the Description page of Project Settings.


#include "Territory/TerritoryConstructButton.h"

#include "SProject.h"
#include "TerritoryDefine.h"
#include "TerritoryPlayerController.h"


void UTerritoryConstructButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTerritoryConstructButton::NativeOnClicked()
{
	Super::NativeOnClicked();
	
	LOG("%s Clicked", *GetName());
	if(const TSubclassOf<ATerritoryBuilding> BuildingBP = LoadObject<UClass>(nullptr, *BuildingPath))
	{
		const TObjectPtr<ATerritoryPlayerController> TerritoryPC = Cast<ATerritoryPlayerController>(GetWorld()->GetFirstPlayerController());
		TerritoryPC->RegisterBuilding(BuildingBP);
		TerritoryPC->SetModeType(ETerritoryModeType::Construct);
	}
}


