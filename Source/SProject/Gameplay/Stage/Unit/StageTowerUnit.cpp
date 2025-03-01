
#include "StageTowerUnit.h"
// include GameCore
#include "AbilitySystem/GAbilitySystemComponent.h"
// include Project
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Unit/Attribute/StageUnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageTowerUnit)

void AStageTowerUnit::InitailizeBaseStats()
{
	TMap<EStageUnitAttribute, double> BaseStats;
	if (auto Err = UStageTableHelper::GetStageTowerBaseStats(GetInfoRef()->TowerType, GetInfoRef()->Kind, GetInfoRef()->Level, BaseStats); !GameCore::IsOK(Err))
	{
		return;
	}

	SetBaseStats(BaseStats);
}


void AStageTowerUnit::SetInfo(FStageTowerInfo NewInfo)
{
	if (!Info.IsValid())
	{
		Info = MakeShared<FStageTowerInfo>();
	}
	*Info = NewInfo;
}

FStageTowerInfo AStageTowerUnit::GetInfo() const
{
	return GetInfoRef().Get();
}

TSharedRef<FStageTowerInfo> AStageTowerUnit::GetInfoRef() const
{
	if (!Info.IsValid())
	{
		return MakeShared<FStageTowerInfo>();
	}
	return Info.ToSharedRef();
}

