
#include "StageTableRepository.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/TowerTable.h"
#include "Table/StageInfoTable.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"

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

	NormalTowerTableRows.Empty();
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

	StageTableRows.Empty();
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

TSortedMap<int32, TSharedPtr<FNormalTowerTableRow>>* UStageTableRepository::FindNormalTowerTableRows(int32 Kind)
{
	auto Result = NormalTowerTableRows.Find(Kind);
	if (!Result)
	{
		UE_LOG(LogTable, Warning, TEXT("NormalTowerTable을 찾지 못하였습니다. [Kind: %d]"), Kind);
		return nullptr;
	}
	return Result;

}

TSharedPtr<FNormalTowerTableRow>* UStageTableRepository::FindNormalTowerTableRow(int32 Kind, int32 Level)
{
	auto KindTable = FindNormalTowerTableRows(Kind);
	if (!KindTable)
	{
		return nullptr;
	}

	auto Result = KindTable->Find(Level);
	if (!Result)
	{
		UE_LOG(LogTable, Warning, TEXT("NormalTowerTable을 찾지 못하였습니다. [Kind: %d, Level: %d]"), Kind, Level);
		return nullptr;
	}

	return Result;
}

//////////////////////////////////////////////////////////////////////////
// UStageTableHelper
//////////////////////////////////////////////////////////////////////////
bool UStageTableHelper::GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FBuildStageTower& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto TowerRow = Repository->FindNormalTowerTableRow(Kind, Level);
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

	default: {
		UE_LOG(LogTable, Warning, TEXT("타워 타입이 잘못되었습니다. [TowerType: %s]"), *UEnum::GetValueAsString(TowerType));
		return false;
	}
	}

	return true;
}

bool UStageTableHelper::GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FBuildStageTower& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	int32 MaxLevel = GetStageTowerMaxLevel(TowerType, Kind);
	if (Level >= MaxLevel)
	{
		return false;
	}

	int32 NextLevel = Level + 1;

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto TowerRow = Repository->FindNormalTowerTableRow(Kind, NextLevel);
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

	default: {
		UE_LOG(LogTable, Warning, TEXT("타워 타입이 잘못되었습니다. [TowerType: %s]"), *UEnum::GetValueAsString(TowerType));
		return false;
	}
	}

	return true;
}

bool UStageTableHelper::GetStageTowerSellPrice(EStageTowerType TowerType, int32 Kind, int32 Level, int64& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);
	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto TowerRow = Repository->FindNormalTowerTableRow(Kind, Level);
		if (!TowerRow)
		{
			return false;
		}
		auto& TowerPtr = *TowerRow;
		Result = TowerPtr->UsePoint;
	}
	break;

	default: {
		UE_LOG(LogTable, Warning, TEXT("타워 타입이 잘못되었습니다. [TowerType: %s]"), *UEnum::GetValueAsString(TowerType));
		return false;
	}
	}

	return true;
}

int32 UStageTableHelper::GetStageTowerMaxLevel(EStageTowerType TowerType, int32 Kind)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	int32 Result = 0;

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto KindTable = Repository->FindNormalTowerTableRows(Kind);
		if (!KindTable || KindTable->IsEmpty())
		{
			return 0;
		}
		Result = (*(--KindTable->end())).Key;
	}
	break;

	default: {
		UE_LOG(LogTable, Warning, TEXT("타워 타입이 잘못되었습니다. [TowerType: %s]"), *UEnum::GetValueAsString(TowerType));
		return 0;
	}
	}

	return FMath::Max(0, Result);
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
