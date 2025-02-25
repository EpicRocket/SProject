
#include "StageSpawner.h"
// include Engine
#include "AIController.h"
#include "Gameplay/ETC/GameplayPathActor.h"
#include "Engine/World.h"
#include "Unit/UnitCharacter.h"
#include "Kismet/GameplayStatics.h"
// include Project
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/GameplayWorldSubsystem.h"

AUnitCharacter* AStageSpawner::Spawn(const FStageSpawnParams& Params)
{
	/*if (!Params.StageLevel.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("StageLevel을 찾지 못하였습니다."));
		return nullptr;
	}

	auto PathActor = Params.StageLevel->GetPathActor(Params.PathPosition);
	if (!PathActor)
	{
		UE_LOG(LogStage, Warning, TEXT("PathActor을 찾지 못하였습니다."));
		return nullptr;
	}

	auto SpawnedUnit = UGameplayHelper::SpawnUnit(this, PathActor->GetActorLocation(), PathActor->GetActorRotation(), Params.SpawnUnit, AAIController::StaticClass());
	SpawnedUnit->SetGenericTeamId(GetGenericTeamId());

	return SpawnedUnit;*/

	return nullptr;
}
