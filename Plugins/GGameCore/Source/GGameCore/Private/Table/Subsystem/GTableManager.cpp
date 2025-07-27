// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "GTableManager.h"
// include Engine
#include "Engine/Engine.h"
// include GameCore
#include "Table/Subsystem/GTableRepositorySubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GTableManager)

void UGTableManager::Initialize(FSubsystemCollectionBase& Collection)
{
	if (GEngine == nullptr)
	{
		return;
	}

	GEngine->ForEachEngineSubsystem<UGTableRepositorySubsystem>(
		[](auto Subsystem) -> void {
			Subsystem->OnStart();
		}
	);
}

void UGTableManager::Deinitialize()
{
	if (GEngine == nullptr)
	{
		return;
	}

	GEngine->ForEachEngineSubsystem<UGTableRepositorySubsystem>(
		[](auto Subsystem) -> void {
			Subsystem->OnEnd();
		}
	);
}
