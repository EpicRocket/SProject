// Fill out your copyright notice in the Description page of Project Settings.


#include "TerritoryPlayerController.h"

#include "IAssetTypeActions.h"
#include "IMediaAudioSample.h"
#include "TerritoryDefine.h"

ATerritoryPlayerController::ATerritoryPlayerController()
{	
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	bEnableTouchEvents = true;
	bEnableTouchOverEvents = true;
}

void ATerritoryPlayerController::SetModeType(const ETerritoryModeType InModeType)
{
	ModeType = InModeType;
	switch (ModeType)
	{
	case ETerritoryModeType::Idle:
		break;
	case ETerritoryModeType::Construct:
		GetWorld()->GetFirstPlayerController()->CurrentClickTraceChannel = ECC_GameTraceChannel1;
		break;
	case ETerritoryModeType::Move:
		break;
	}
}