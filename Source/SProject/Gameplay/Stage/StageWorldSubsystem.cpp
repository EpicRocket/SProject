
#include "StageWorldSubsystem.h"

#include "StageLogging.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageWorldSubsystem)

void UStageWorldSubsystem::AddLevel(int32 StageKey, AStageLevel* Level)
{
	//TWeakObjectPtr<AStageLevel> LevelPtr = Level;
	/*if (!LevelPtr.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("Level이 유효하지 않습니다."));
		return;
	}*/

	//ActivatableLevels.Emplace(StageKey, LevelPtr);
}

void UStageWorldSubsystem::RemoevLevel(int32 StageKey)
{
	//ActivatableLevels.Remove(StageKey);
}
