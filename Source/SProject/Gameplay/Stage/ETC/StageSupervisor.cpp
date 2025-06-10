// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageSupervisor.h"
// include Project
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageLogging.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSupervisor)

void AStageSupervisor::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerLevel.IsValid())
	{
		UE_LOG(LogStage, Error, TEXT("OwnerLevel를 찾을 수 없습니다."));
		return;
	}

	//OwnerLevel->GetSpawner()
}

void AStageSupervisor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{


	Super::EndPlay(EndPlayReason);
}
