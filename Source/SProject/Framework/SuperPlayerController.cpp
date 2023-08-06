// Copyright 2023. GreenTeaRocket all rights reserved.

#include "SuperPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"

void ASuperPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(MappingContext, MappingPriority);
	}
}
