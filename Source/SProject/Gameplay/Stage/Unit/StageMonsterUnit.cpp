// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageMonsterUnit.h"
// include Project
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/StageMonsterTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageMonsterUnit)

void AStageMonsterUnit::OnInit()
{
}

void AStageMonsterUnit::OnInitBaseStats()
{
	if (!IsValid(MonsterInfoContext))
	{
		UE_LOGFMT(LogStage, Warning, "%s: MonsterInfoContext not found", *GetName());
		return;
	}

	TMap<EStageUnitAttribute, double> BaseStats;
	auto Err = UStageTableHelper::GetStageMonsterBaseStats(
		this,
		MonsterInfoContext->MonsterInfo.Index,
		BaseStats
	);
	if (!GameCore::IsOK(Err))
	{
		return;
	}

	SetBaseStats(BaseStats);
}

void AStageMonsterUnit::Setup(FStageMonsterInfo NewMonsterInfo)
{
	EObjectFlags Flags = RF_Transient | RF_Public;
	MonsterInfoContext = NewObject<UStageMonsterContext>(this, NAME_None, Flags);
	MonsterInfoContext->MonsterInfo = NewMonsterInfo;
}

FGErrorInfo AStageMonsterUnit::K2_GetMonsterInfo(FStageMonsterInfo& MonsterInfo)
{
	if(!IsValid(MonsterInfoContext))
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("MonsterInfoContext"));
	}

	MonsterInfo = MonsterInfoContext->MonsterInfo;
	return GameCore::Pass();
}

TOptional<FStageMonsterInfo> AStageMonsterUnit::GetMonsterInfo() const
{
	if (!IsValid(MonsterInfoContext))
	{
		return TOptional<FStageMonsterInfo>{};
	}
	return MonsterInfoContext->MonsterInfo;
}
