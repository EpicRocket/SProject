
#include "StageSpawnComponent.h"
// include Engine
#include "Engine/World.h"
#include "AIController.h"
// include Project
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSpawnComponent)

AStageTowerUnit* UStageSpawnComponent::SpawnTower(AStageLevel* TargetLevel, FVector Location, FRotator Rotation, FBuildStageTower Build, TSubclassOf<AAIController> AIController)
{
    if (!IsValid(TargetLevel))
    {
        UE_LOGFMT(LogStage, Warning, "TargetLevel이 없어 타워 생성에 실패하였습니다.");
        return nullptr;
    }



    return nullptr;
}
