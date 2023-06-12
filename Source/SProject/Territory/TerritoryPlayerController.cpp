// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppMemberFunctionMayBeStatic
#include "TerritoryPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TerritoryDefine.h"
#include "TerritoryInputConfigData.h"
#include "Engine/LocalPlayer.h"

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
	
	if(const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		SubSystem->ClearAllMappings();
		SubSystem->AddMappingContext(InputMapping, 0);
	}

	if(const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if(InputConfigData)
		{
			EnhancedInputComponent->BindAction(InputConfigData->InputPick, ETriggerEvent::Triggered, this, &ATerritoryPlayerController::OnTouchBegin);
			EnhancedInputComponent->BindAction(InputConfigData->InputPick, ETriggerEvent::Triggered, this, &ATerritoryPlayerController::OnHold);
			EnhancedInputComponent->BindAction(InputConfigData->InputPick, ETriggerEvent::Triggered, this, &ATerritoryPlayerController::OnTouchEnd);
			EnhancedInputComponent->BindAction(InputConfigData->InputPick, ETriggerEvent::Triggered, this, &ATerritoryPlayerController::OnTap);
		}
	}
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

void ATerritoryPlayerController::OnTouchBegin(const FInputActionValue& ActionValue)
{
}

void ATerritoryPlayerController::OnHold(const FInputActionValue& ActionValue)
{
}

void ATerritoryPlayerController::OnTouchEnd(const FInputActionValue& ActionValue)
{
}

void ATerritoryPlayerController::OnTap(const FInputActionValue& ActionValue)
{
}
