

#include "StageMonsterUnit.h"

// include Project
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/StageMonsterTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageMonsterUnit)

void AStageMonsterUnit::InitailizeBaseStats()
{
	TMap<EStageUnitAttribute, double> BaseStats;
	if (auto Err = UStageTableHelper::GetStageMonsterBaseStats(GetInfoRef()->Index, BaseStats); !GameCore::IsOK(Err))
	{
		return;
	}

	SetBaseStats(BaseStats);
}

void AStageMonsterUnit::SetInfo(FStageMonsterInfo NewInfo)
{
	if (!Info.IsValid())
	{
		Info = MakeShared<FStageMonsterInfo>();
	}
	*Info = NewInfo;
}

FStageMonsterInfo AStageMonsterUnit::GetInfo() const
{
	return GetInfoRef().Get();
}

TSharedRef<FStageMonsterInfo> AStageMonsterUnit::GetInfoRef() const
{
	if (!Info.IsValid())
	{
		return MakeShared<FStageMonsterInfo>();
	}
	return Info.ToSharedRef();
}

