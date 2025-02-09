
#include "StageSpawnComponent.h"
// include Engine
#include "Engine/World.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
// include Project
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSpawnComponent)

AStageTowerUnit* UStageSpawnComponent::SpawnTower(AStageLevel* TargetLevel, FVector Location, FRotator Rotation, FBuildStageTower Build, TSubclassOf<AAIController> AIController)
{
    if (!IsValid(TargetLevel))
    {
        UE_LOGFMT(LogStage, Warning, "TargetLevel이 없어 타워 생성에 실패하였습니다.");
        return nullptr;
    }

    auto World = TargetLevel->GetWorld();
    if (!World)
    {
		UE_LOGFMT(LogStage, Warning, "World가 없어 타워 생성에 실패하였습니다.");
        return nullptr;
    }

    TSubclassOf<AStageTowerUnit> UnitClass;
	if (!UStageTableHelper::GetStageTowerUnitClass(Build.TowerType, Build.Kind, Build.Level, UnitClass))
	{
		return nullptr;
	}

    AStageTowerUnit* SpawnUnit = World->SpawnActorDeferred<AStageTowerUnit>(
        UnitClass,
        FTransform(Rotation, Location),
        TargetLevel,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );
    SpawnUnit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    if (AIController)
    {
		SpawnUnit->AIControllerClass = AIController;
    }

	// NOTE. 타워 기본 정보 설정
	SpawnUnit->SetInfo(Build);

    float HalfHeight = SpawnUnit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    SpawnUnit->FinishSpawning(
        FTransform(Rotation, Location + HalfHeight),
        false,
        nullptr,
        ESpawnActorScaleMethod::MultiplyWithRoot);

    return SpawnUnit;
}
