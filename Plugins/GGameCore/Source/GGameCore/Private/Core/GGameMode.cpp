// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Core/GGameMode.h"
// include Engine
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameMode)

void AGGameMode::BeginPlay()
{
	Super::BeginPlay();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}
