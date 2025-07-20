// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageStartPoint.h"
// include Project
#include "Gameplay/Stage/ETC/StageSupervisor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStartPoint)

void AStageStartPoint::Setup(AStageSupervisor* InSupervisor)
{
	Supervisor = InSupervisor;
}
