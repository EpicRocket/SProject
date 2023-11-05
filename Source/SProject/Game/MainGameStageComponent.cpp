
#include "MainGameStageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MainGameStageComponent)

void UMainGameStageComponent::RegisterGameStage(int32 GameStageId, FMainGameStageData GameStageData)
{
	MainGameStageDataMap.Emplace(GameStageId, GameStageData);
}

const FMainGameStageData& UMainGameStageComponent::GetMainGameStageData(int32 GameStageId) const
{
	if (MainGameStageDataMap.Contains(GameStageId))
	{
		return MainGameStageDataMap[GameStageId];
	}
	else
	{
		return Global::GetEmpty<FMainGameStageData>();
	}
}
