// Fill out your copyright notice in the Description page of Project Settings.


#include "TerritoryPlayerController.h"

#include "TerritoryDefine.h"

ATerritoryPlayerController::ATerritoryPlayerController()
{	
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	bEnableTouchEvents = true;
	bEnableTouchOverEvents = true;
}

void ATerritoryPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	
}

void ATerritoryPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void ATerritoryPlayerController::SetModeType(const ETerritoryModeType InModeType)
{
	ModeType = InModeType;
	switch (ModeType)
	{
	case ETerritoryModeType::Idle:
		CurrentClickTraceChannel = ECC_Visibility;
		break;
	case ETerritoryModeType::Construct:
		CurrentClickTraceChannel = ECC_GameTraceChannel1;
		break;
	case ETerritoryModeType::Move:
		CurrentClickTraceChannel = ECC_GameTraceChannel1;
		break;
	}	
}