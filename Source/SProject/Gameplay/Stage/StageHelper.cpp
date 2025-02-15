
#include "StageHelper.h"
// include Engine
#include "Engine/Engine.h"
#include "GameFramework/GameState.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/StageTable.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/Component/StageStateComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageHelper)


/*static*/ bool UStageHelper::GetCurrentStageLevel(const UObject* WorldContextObject, int32& Level)
{
    Level = INDEX_NONE;
    if (!IsValid(WorldContextObject))
    {
        return false;
    }

    auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return false;
	}
    
    auto GameState = World->GetGameState();
	if (!IsValid(GameState))
	{
		return false;
	}

    auto StageComponent = GameState->GetComponentByClass<UStageStateComponent>();
	if (!IsValid(StageComponent))
	{
		return false;
	}

    if (!StageComponent->TargetStage.IsValid())
    {
        return false;
    }

	Level = StageComponent->TargetStage->Level;
    return Level != INDEX_NONE;
}

/*static*/ bool UStageHelper::GetCurrentStageUsePoint(const UObject* WorldContextObject, int32& UsePoint)
{
    UsePoint = NULL;
    if (!IsValid(WorldContextObject))
    {
        return false;
    }

    auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!IsValid(World))
    {
        return false;
    }

    auto GameState = World->GetGameState();
    if (!IsValid(GameState))
    {
        return false;
    }

    auto StageComponent = GameState->GetComponentByClass<UStageStateComponent>();
    if (!IsValid(StageComponent))
    {
        return false;
    }

    if (!StageComponent->TargetStage.IsValid())
    {
        return false;
    }

    auto Row = UTableHelper::GetData<FStageTableRow>(StageComponent->TargetStage->Level);
    if (!Row)
    {
        return false;
    }

	UsePoint = Row->UsePoint;
    return UsePoint != NULL;
}
