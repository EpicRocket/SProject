
#include "StageTableRepository.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/TowerTable.h"
#include "Table/StageInfoTable.h"
#include "TableTypes/StageTableTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageTableRepository)

//////////////////////////////////////////////////////////////////////////
// UStageTableRepository
//////////////////////////////////////////////////////////////////////////
UStageTableRepository* UStageTableRepository::Get()
{
	if (GEngine == NULL)
	{
		return nullptr;
	}
	return GEngine->GetEngineSubsystem<UStageTableRepository>();
}

void UStageTableRepository::Load()
{
	auto TableSubsystem = UTableSubsystem::Get();
	if (!TableSubsystem)
	{
		return;
	}

	for (auto Row : TableSubsystem->GetTableDatas<FNormalTowerTableRow>())
	{
		if (Row == nullptr)
		{
			continue;
		}

		auto& Datas = NormalTowerTableRows.FindOrAdd(Row->Kind);
		if (!Datas.Contains(Row->Level))
		{
			Datas.Emplace(Row->Level, MakeShared<FNormalTowerTableRow>(*Row));
		}
	}

	for (auto Row : TableSubsystem->GetTableDatas<FStageTableRow>())
	{
		if (Row == nullptr)
		{
			continue;
		}

		StageTableRows.Emplace(Row->Level, MakeShared<FStageTableRow>(*Row));
	}
}

void UStageTableRepository::Unload()
{
	NormalTowerTableRows.Empty();
	StageTableRows.Empty();
}

//////////////////////////////////////////////////////////////////////////
// UStageTableHelper
//////////////////////////////////////////////////////////////////////////
bool UStageTableHelper::GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FBuildStageTower& Result)
{
	auto Repository = UStageTableRepository::Get();
	if (!Repository)
	{
		return false;
	}

	switch (TowerType)
	{
	case EStageTowerType::Normal:
	{
		auto KindTable = Repository->NormalTowerTableRows.Find(Kind);
		if (!KindTable)
		{
			return false;
		}

		auto TowerRow = KindTable->Find(Level);
		if (!TowerRow)
		{
			return false;
		}

		auto& TowerPtr = *TowerRow;
		Result.Index = TowerPtr->Index;
		Result.Kind = TowerPtr->Kind;
		Result.Level = TowerPtr->Level;
		Result.Name = TowerPtr->Name;
		// FIXME: 텍스쳐 로드는 로딩에서 미리 해둬야 함.
		Result.Icon = TowerPtr->IconPath.LoadSynchronous();
	}
	break;

	default: return false;
	}

	return true;
}

bool UStageTableHelper::GetStageTableInfo(int32 Level, FStageTableRow& Result)
{
	auto Repository = UStageTableRepository::Get();
	if (!Repository)
	{
		return false;
	}

	auto StageInfoRow = Repository->StageTableRows.Find(Level);
	if (!StageInfoRow)
	{
		return false;
	}

	auto& StageInfoPtr = *StageInfoRow;
	Result.Level = StageInfoPtr->Level;
	Result.UsePoint = StageInfoPtr->UsePoint;

	return true;
}
