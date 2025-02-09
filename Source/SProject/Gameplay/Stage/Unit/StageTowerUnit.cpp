
#include "StageTowerUnit.h"
// include GameCore
#include "AbilitySystem/GAbilitySystemComponent.h"
// include Project
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Unit/Attribute/StageUnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageTowerUnit)

void AStageTowerUnit::SetInfo(FBuildStageTower NewInfo)
{
	Info = MakeShared<FBuildStageTower>(NewInfo);
}

FBuildStageTower AStageTowerUnit::GetInfo() const
{
	return GetInfoRef().Get();
}

TSharedRef<FBuildStageTower> AStageTowerUnit::GetInfoRef() const
{
	if (!Info.IsValid())
	{
		return MakeShared<FBuildStageTower>();
	}
	return Info.ToSharedRef();
}

void AStageTowerUnit::InitailizeBaseStats()
{
	TMap<EStageUnitAttribute, double> BaseStats;
	if (UStageTableHelper::GetStageTowerBaseStats(GetInfoRef()->TowerType, GetInfoRef()->Kind, GetInfoRef()->Level, BaseStats))
	{
		SetBaseStats(BaseStats);
	}
}
