
#include "StageTableRepository.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/TowerTable.h"
#include "Table/StageTable.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"
#include "Gameplay/Stage/Unit/Attribute/StageUnitAttributeSet.h"

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
		UE_LOGFMT(LogTable, Warning, "NormalTowerTableRow을 찾지 못하였습니다. [Kind: {Kind}]", ("Kind", Kind));
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
		UE_LOGFMT(LogTable, Warning, "NormalTowerTableRow을 찾지 못하였습니다. [Kind: {Kind}, Level: {Level}]", ("Kind", Kind), ("Level", Level));
		return nullptr;
	}

	return Result;
}

TSharedPtr<FStageTableRow> UStageTableRepository::FindStageTableRow(int32 Level)
{
	auto Row = StageTableRows.Find(Level);
	if (!Row)
	{
		UE_LOGFMT(LogTable, Warning, "StageTableRow을 찾지 못하였습니다. [Level:{Level}]", ("Level", Level));
		return nullptr;
	}

	return *Row;
}

//////////////////////////////////////////////////////////////////////////
// UStageTableHelper
//////////////////////////////////////////////////////////////////////////
bool UStageTableHelper::GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
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
		Result.Icon = TowerPtr->Icon.LoadSynchronous();
	}
	break;

	default: {
		UE_LOG(LogTable, Warning, TEXT("타워 타입이 잘못되었습니다. [TowerType: %s]"), *UEnum::GetValueAsString(TowerType));
		return false;
	}
	}

	return true;
}

bool UStageTableHelper::GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
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
		Result.Icon = TowerPtr->Icon.LoadSynchronous();
	} break;

	default: {
		UE_LOG(LogTable, Warning, TEXT("타워 타입이 잘못되었습니다. [TowerType: %s]"), *UEnum::GetValueAsString(TowerType));
		return false;
	}
	}

	return true;
}

bool UStageTableHelper::GetStageTowerUnitClass(EStageTowerType TowerType, int32 Kind, int32 Level, TSubclassOf<AStageTowerUnit>& Result)
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

		Result = (*TowerRow)->Unit.LoadSynchronous();

	} break;

	default: {
		UE_LOG(LogTable, Warning, TEXT("타워 타입이 잘못되었습니다. [TowerType: %s]"), *UEnum::GetValueAsString(TowerType));
		return false;
	}
	}

	if (!Result)
	{
		UE_LOGFMT(LogTable, Warning,
			"타워 유닛 클래스를 찾지 못하였습니다. [TowerType: {Type}, Kind: {Kind}, Level: {Level}]",
			("Type",  * UEnum::GetValueAsString(TowerType)),
			("Kind", Kind),
			("Level", Level));

		return false;
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
	} break;

	default: {
		UE_LOG(LogTable, Warning, TEXT("타워 타입이 잘못되었습니다. [TowerType: %s]"), *UEnum::GetValueAsString(TowerType));
		return 0;
	}
	}

	return FMath::Max(0, Result);
}

bool UStageTableHelper::GetStageTowerBaseStats(EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result)
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

		Result.FindOrAdd(EStageUnitAttribute::Level) = (*TowerRow)->Level;
		Result.FindOrAdd(EStageUnitAttribute::Grade) = (*TowerRow)->Grade;
		Result.FindOrAdd(EStageUnitAttribute::Attack) = (*TowerRow)->Attack;
		Result.FindOrAdd(EStageUnitAttribute::Defence) = (*TowerRow)->Defence;
		Result.FindOrAdd(EStageUnitAttribute::MaxHp) = (*TowerRow)->Hp;
		Result.FindOrAdd(EStageUnitAttribute::Hp) = (*TowerRow)->Hp;
		Result.FindOrAdd(EStageUnitAttribute::AttackSpeed) = (*TowerRow)->AttackSpeed;
		Result.FindOrAdd(EStageUnitAttribute::MovementSpeed) = 0.0;
		Result.FindOrAdd(EStageUnitAttribute::Range) = (*TowerRow)->Range;
		Result.FindOrAdd(EStageUnitAttribute::SplashScale) = (*TowerRow)->SplashScale;

	} break;

	default: {
		UE_LOG(LogTable, Warning, TEXT("타워 타입이 잘못되었습니다. [TowerType: %s]"), *UEnum::GetValueAsString(TowerType));
		return false;
	}
	}

	return true;
}

bool UStageTableHelper::GetStage(int32 Level, FStageTableRow& Result)
{
	auto Repo = UStageTableRepository::Get();
	check(Repo);

	auto Row = Repo->StageTableRows.Find(Level);
	if (!Row)
	{
		return false;
	}

	auto& StageInfoPtr = *Row;
	Result.Level = StageInfoPtr->Level;
	Result.UsePoint = StageInfoPtr->UsePoint;

	return true;
}

bool UStageTableHelper::GetStageMap(int32 Level, TSoftObjectPtr<UWorld>& Map)
{
	auto Repo = UStageTableRepository::Get();
	check(Repo);

	auto Row = Repo->FindStageTableRow(Level);
	if (!Row)
	{
		return false;
	}

	Map = Row->Map;
	if (Map.IsNull())
	{
		UE_LOGFMT(LogTable, Warning, "Map이 Null입니다.[Level:{Level}]", ("Level", Level));
		return false;
	}

	if (Map.IsValid())
	{
		UE_LOGFMT(LogTable, Warning, "Map이 유효하지 않습니다.[Level:{Level}]", ("Level", Level));
		return false;
	}

	return true;
}
