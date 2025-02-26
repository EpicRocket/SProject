
#include "StageTableRepository.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
// include GameCore
#include "Error/GError.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/TowerTable.h"
#include "Table/MonsterTable.h"
#include "Table/StageTable.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Types/StageMonsterTypes.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"
#include "Gameplay/Stage/Unit/StageMonsterUnit.h"
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
	Super::Load();

	SCOPED_BOOT_TIMING("UStageTableRepository::Load");
	const double StartTime = FPlatformTime::Seconds();

	NormalTowerTableRows.Empty();
	for (auto Row : UTableHelper::GetDatas<FNormalTowerTableRow>())
	{
		auto& Datas = NormalTowerTableRows.FindOrAdd(Row->Kind);
		if (!Datas.Contains(Row->Level))
		{
			Datas.Emplace(Row->Level, MakeShared<FNormalTowerTableRow>(*Row));
		}
	}

	{
		TArray<int32> Keys;
		MonsterInfos.GetKeys(Keys);
		for (auto Row : UTableHelper::GetDatas<FMonsterTableRow>())
		{
			Keys.Remove(Row->Index);
			TSharedPtr<FStageMonsterInfo> Ptr;
			if (MonsterInfos.Contains(Row->Index))
			{
				Ptr = MonsterInfos[Row->Index];
			}
			else
			{
				Ptr = MakeShared<FStageMonsterInfo>();
				Ptr->Index = Row->Index;
				MonsterInfos.Emplace(Row->Index, Ptr);
			}

			Ptr->Level = Row->Level;
			Ptr->Grade = Row->Grade;
			Ptr->Name = Row->Name;
			Ptr->AttackType = Row->AttackType;
			Ptr->UnitClass = Row->Unit.LoadSynchronous();
			Ptr->Icon = Row->Icon.LoadSynchronous();
		}

		for (auto Key : Keys)
		{
			MonsterInfos.Remove(Key);
		}
	}

	StageTableRows.Empty();
	for (auto Row : UTableHelper::GetDatas<FStageTableRow>())
	{
		StageTableRows.Emplace(Row->Level, MakeShared<FStageTableRow>(*Row));
	}

	UE_LOG(LogTable, Display, TEXT("StageTableRepository 로드 완료(%.2f)"), FPlatformTime::Seconds() - StartTime);
}

void UStageTableRepository::Unload()
{
	Super::Unload();

	NormalTowerTableRows.Empty();
	MonsterInfos.Empty();
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

TSharedPtr<FStageMonsterInfo> UStageTableRepository::FindMonsterInfo(int32 MonsterKey)
{
	auto Result = MonsterInfos.Find(MonsterKey);
	if (!Result)
	{
		UE_LOGFMT(LogTable, Warning, "MonsterInfo을 찾지 못하였습니다. [MonsterKey: {MonsterKey}]", ("MonsterKey", MonsterKey));
		return nullptr;
	}
	return *Result;
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
FGErrorInfo UStageTableHelper::GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto TowerRow = Repository->FindNormalTowerTableRow(Kind, Level);
		if (!TowerRow)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("FNormalTowerTableRow"));
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
		return GameCore::Throw(GameErr::ENUM_INVALID, UEnum::GetValueAsString(TowerType));
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	int32 MaxLevel = GetStageTowerMaxLevel(TowerType, Kind);
	if (Level >= MaxLevel)
	{
		return GameCore::Throw(GameErr::VALUE_INVALID, FString::Printf(TEXT("Level:%d, MaxLevel:%d"), Level, MaxLevel));
	}

	int32 NextLevel = Level + 1;

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto TowerRow = Repository->FindNormalTowerTableRow(Kind, NextLevel);
		if (!TowerRow)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("FNormalTowerTableRow"));
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
		return GameCore::Throw(GameErr::ENUM_INVALID, UEnum::GetValueAsString(TowerType));
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageTowerUnitClass(EStageTowerType TowerType, int32 Kind, int32 Level, TSubclassOf<AStageTowerUnit>& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto TowerRow = Repository->FindNormalTowerTableRow(Kind, Level);
		if (!TowerRow)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("FNormalTowerTableRow"));
		}

		Result = (*TowerRow)->Unit.LoadSynchronous();

	} break;

	default: {
		return GameCore::Throw(GameErr::ENUM_INVALID, UEnum::GetValueAsString(TowerType));
	}
	}

	if (!Result)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("AStageTowerUnit"));
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageTowerSellPrice(EStageTowerType TowerType, int32 Kind, int32 Level, int64& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);
	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto TowerRow = Repository->FindNormalTowerTableRow(Kind, Level);
		if (!TowerRow)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("FNormalTowerTableRow"));
		}
		auto& TowerPtr = *TowerRow;
		Result = TowerPtr->UsePoint;
	}
	break;

	default: {
		return GameCore::Throw(GameErr::ENUM_INVALID, UEnum::GetValueAsString(TowerType));
	}
	}

	return GameCore::Pass();
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

FGErrorInfo UStageTableHelper::GetStageTowerBaseStats(EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto TowerRow = Repository->FindNormalTowerTableRow(Kind, Level);
		if (!TowerRow)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("FNormalTowerTableRow"));
		}

		Result.Empty();
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
		return GameCore::Throw(GameErr::ENUM_INVALID, UEnum::GetValueAsString(TowerType));
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMonsterInfo(int32 MonsterKey, FStageMonsterInfo& Result)
{
	auto Repo = UStageTableRepository::Get();
	check(Repo);

	TSharedPtr<FStageMonsterInfo> MonsterInfo = Repo->FindMonsterInfo(MonsterKey);
	if (!MonsterInfo.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FStageMonsterInfo find MonsterKey %d"), MonsterKey));
	}

	Result = *MonsterInfo;
	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMonsterBaseStats(int32 MonsterKey, TMap<EStageUnitAttribute, double>& Result)
{
	auto Row = UTableHelper::GetData<FMonsterTableRow>(MonsterKey);
	if (!Row)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FMonsterTableRow find MonsterKey %d"), MonsterKey));
	}

	Result.Empty();
	Result.FindOrAdd(EStageUnitAttribute::Level) = Row->Level;
	Result.FindOrAdd(EStageUnitAttribute::Grade) = Row->Grade;
	Result.FindOrAdd(EStageUnitAttribute::Attack) = Row->Attack;
	Result.FindOrAdd(EStageUnitAttribute::Defence) = Row->Defence;
	Result.FindOrAdd(EStageUnitAttribute::MaxHp) = Row->Hp;
	Result.FindOrAdd(EStageUnitAttribute::Hp) = Row->Hp;
	Result.FindOrAdd(EStageUnitAttribute::AttackSpeed) = Row->AttackSpeed;
	Result.FindOrAdd(EStageUnitAttribute::MovementSpeed) = Row->MovementSpeed;
	Result.FindOrAdd(EStageUnitAttribute::Range) = Row->Range;

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStage(int32 Level, FStageTableRow& Result)
{
	auto Repo = UStageTableRepository::Get();
	check(Repo);

	auto Row = Repo->StageTableRows.Find(Level);
	if (!Row)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FStageTableRow find Level %d"), Level));
	}

	auto& StageInfoPtr = *Row;
	Result.Level = StageInfoPtr->Level;
	Result.UsePoint = StageInfoPtr->UsePoint;

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMap(int32 Level, TSoftObjectPtr<UWorld>& Map)
{
	auto Repo = UStageTableRepository::Get();
	check(Repo);

	auto Row = Repo->FindStageTableRow(Level);
	if (!Row)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FStageTableRow find Level %d"), Level));
	}

	Map = Row->Map;
	if (Map.IsNull())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("Level: %d"), Level));
	}

	if (Map.IsValid())
	{
		return GameCore::Throw(GameErr::OBJECT_INVALID, FString::Printf(TEXT("Level: %d"), Level));
	}

	return GameCore::Pass();
}
