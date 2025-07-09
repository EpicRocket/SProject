// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StagePlayerPawn.h"
// include Engine
#include "GameFramework/Controller.h"
// include Project
#include "Gameplay/Stage/Stage.h"
#include "Gameplay/Stage/StageLevel.h"
#include "ETC/StageBuildZone.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePlayerPawn)


void AStagePlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!NewController->IsLocalPlayerController())
	{
		return;
	}

	OnUpdateState();
}

void AStagePlayerPawn::UnPossessed()
{
	Super::UnPossessed();

	OnUpdateState();
}

void AStagePlayerPawn::OnUpdateState()
{
}
