
#include "StageTableRepository.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
#include "BehaviorTree/BehaviorTree.h"
// include GameCore
#include "Error/GError.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/TowerTable.h"
#include "Table/MonsterTable.h"
#include "Table/StageTable.h"
#include "Table/MonsterGroupTable.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Types/StageMonsterTypes.h"
#include "Gameplay/Stage/Types/StageWaveTypes.h"
#include "Gameplay/Stage/Types/StageMonsterGroupTypes.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"
#include "Gameplay/Stage/Unit/StageMonsterUnit.h"
#include "Gameplay/Stage/Unit/StageUnitAttributeSet.h"

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

	// 타워
	{
		TMap<int32, TSharedPtr<FNormalTowerTableRow>> Rows;
		for (auto& [_, Map] : NormalTowerTableRows)
		{
			for (auto& [__, Row] : Map)
			{
				Rows.Emplace(Row->Index, Row);
			}
		}
		NormalTowerTableRows.Empty();

		TMap<int32, TSharedPtr<FStageTowerInfo>> Infos;
		for (auto& [_, Map] : NormalTowerInfos)
		{
			for (auto& [__, Info] : Map)
			{
				Infos.Emplace(Info->Index, Info);
			}
		}
		NormalTowerInfos.Empty();

		for (auto Row : UTableHelper::GetDatas<FNormalTowerTableRow>())
		{
			auto& Datas = NormalTowerTableRows.FindOrAdd(Row->Kind);
			if (Rows.Contains(Row->Index))
			{
				auto& OldRow = Rows[Row->Index];
				*OldRow = *Row;
				Datas.Emplace(Row->Level, OldRow);
			}
			else
			{
				Datas.Emplace(Row->Level, MakeShared<FNormalTowerTableRow>(*Row));
			}

			auto& TowerInfos = NormalTowerInfos.FindOrAdd(Row->Kind);
			TSharedPtr<FStageTowerInfo> Ptr = nullptr;
			if (Infos.Contains(Row->Index))
			{
				Ptr = Infos[Row->Index];
			}
			else
			{
				Ptr = MakeShared<FStageTowerInfo>();
			}

			Ptr->TowerType = EStageTowerType::Normal;
			Ptr->Index = Row->Index;
			Ptr->Kind = Row->Kind;
			Ptr->Level = Row->Level;
			Ptr->UsePoint = Row->UsePoint;
			Ptr->Name = Row->Name;
			Ptr->AttackType = Row->AttackType;
			Ptr->UnitClass = Row->Unit.LoadSynchronous();
			Ptr->Icon = Row->Icon.LoadSynchronous();
			Ptr->AI = Row->AI.LoadSynchronous();
			TowerInfos.Emplace(Row->Level, Ptr);
		}
	}

	// 몬스터
	{
		TMap<int32, TSharedPtr<FStageMonsterInfo>> Infos;
		for (auto& [_, Info] : MonsterInfos)
		{
			Infos.Emplace(Info->Index, Info);
		}
		MonsterInfos.Empty();

		for (auto Row : UTableHelper::GetDatas<FMonsterTableRow>())
		{
			TSharedPtr<FStageMonsterInfo> Ptr;
			if (Infos.Contains(Row->Index))
			{
				Ptr = Infos[Row->Index];
			}
			else
			{
				Ptr = MakeShared<FStageMonsterInfo>();
			}

			Ptr->Index = Row->Index;
			Ptr->Level = Row->Level;
			Ptr->Grade = Row->Grade;
			Ptr->Name = Row->Name;
			Ptr->AttackType = Row->AttackType;
			Ptr->UnitClass = Row->Unit.LoadSynchronous();
			Ptr->Icon = Row->Icon.LoadSynchronous();
			Ptr->AI = Row->AI.LoadSynchronous();

			MonsterInfos.Emplace(Row->Index, Ptr);
		}
	}

	// 몬스터 그룹
	{
		TMap<int32, TSharedPtr<FMonsterGroupTableRow>> Infos;
		for (auto& [_, Array] : MonsterGroupInfos)
		{
			for (auto& Info : Array)
			{
				Infos.Emplace(Info->Index, Info);
			}
		}
		MonsterGroupInfos.Empty();

		for (auto Row : UTableHelper::GetDatas<FMonsterGroupTableRow>())
		{
			TSharedPtr<FMonsterGroupTableRow> Ptr;
			if (Infos.Contains(Row->Index))
			{
				Ptr = Infos[Row->Index];
			}
			else
			{
				Ptr = MakeShared<FMonsterGroupTableRow>(*Row);
			}

			if (MonsterGroupInfos.Contains(Row->Group))
			{
				MonsterGroupInfos.Find(Row->Group)->Add(Ptr);
			}
			else
			{
				MonsterGroupInfos.Emplace(Row->Group, { Ptr });
			}
		}
	}

	// 웨이브
	{	
		TMap<int32, TSharedPtr<FStageWaveGroupInfo>> Infos;
		for (auto& [_, Array] : WaveGroupInfos)
		{
			for (auto& Info : Array)
			{
				Infos.Emplace(Info->Index, Info);
			}
		}
		WaveGroupInfos.Empty();

		for (auto Row : UTableHelper::GetDatas<FWaveTableRow>())
		{
			TSharedPtr<FStageWaveGroupInfo> Ptr;
			if (Infos.Contains(Row->Index))
			{
				Ptr = Infos[Row->Index];
			}
			else
			{
				Ptr = MakeShared<FStageWaveGroupInfo>();
			}

			Ptr->Index = Row->Index;
			Ptr->Type = Row->Type;
			Ptr->MonsterGroup = Row->Monster_Group;
			
			if (WaveGroupInfos.Contains(Row->Wave_Group))
			{
				WaveGroupInfos.Find(Row->Wave_Group)->Add(Ptr);
			}
			else
			{
				WaveGroupInfos.Emplace(Row->Wave_Group, { Ptr });
			}
		}
	}

	UE_LOG(LogTable, Display, TEXT("StageTableRepository 로드 완료(%.2f)"), FPlatformTime::Seconds() - StartTime);
}

void UStageTableRepository::Unload()
{
	Super::Unload();

	NormalTowerTableRows.Empty();
	MonsterInfos.Empty();
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

TSortedMap<int32, TSharedPtr<FStageTowerInfo>>* UStageTableRepository::FindNormalTowerTableInfos(int32 Kind)
{
	auto Result = NormalTowerInfos.Find(Kind);
	if (!Result)
	{
		UE_LOGFMT(LogTable, Warning, "NormalTowerInfo을 찾지 못하였습니다. [Kind: {Kind}]", ("Kind", Kind));
		return nullptr;
	}

	return Result;
}

TSharedPtr<FStageTowerInfo>* UStageTableRepository::FindNormalTowerInfo(int32 Kind, int32 Level)
{
	auto KindTable = FindNormalTowerTableInfos(Kind);
	if (!KindTable)
	{
		return nullptr;
	}

	auto Result = KindTable->Find(Level);
	if (!Result)
	{
		UE_LOGFMT(LogTable, Warning, "NormalTowerInfo을 찾지 못하였습니다. [Kind: {Kind}, Level: {Level}]", ("Kind", Kind), ("Level", Level));
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

TArray<TSharedPtr<FMonsterGroupTableRow>>* UStageTableRepository::FindMonsterGroupInfo(int32 Group)
{
	auto Result = MonsterGroupInfos.Find(Group);
	if (!Result)
	{
		UE_LOGFMT(LogTable, Warning, "MonsterGroup Info을 찾지 못하였습니다. [MonsterGroup: {Group}]", ("Group", Group));
		return nullptr;
	}
	return Result;
}

TArray<TSharedPtr<FStageWaveGroupInfo>>* UStageTableRepository::FindWaveGroupInfo(int32 WaveGroup)
{
	auto Result = WaveGroupInfos.Find(WaveGroup);
	if (!Result)
	{
		UE_LOGFMT(LogTable, Warning, "WaveGroup Info을 찾지 못하였습니다. [WaveGroup: {WaveGroup}]", ("WaveGroup", WaveGroup));
		return nullptr;
	}
	return Result;
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
		auto Info = Repository->FindNormalTowerInfo(Kind, Level);
		if (!Info)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("FStageTowerInfo"));
		}
		Result = **Info;
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
		auto Info = Repository->FindNormalTowerInfo(Kind, NextLevel);
		if (!Info)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("FStageTowerInfo"));
		}
		Result = **Info;
	} break;

	default: {
		return GameCore::Throw(GameErr::ENUM_INVALID, UEnum::GetValueAsString(TowerType));
	}
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
		auto Info = Repository->FindNormalTowerInfo(Kind, Level);
		if (!Info)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("FStageTowerInfo"));
		}
		Result = (*Info)->UsePoint;
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
	auto Row = UTableHelper::GetData<FStageTableRow>(Level);
	if (!Row)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FStageTableRow find Level %d"), Level));
	}
	Result = *Row;
	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMap(int32 Level, TSoftObjectPtr<UWorld>& Map)
{
	auto Row = UTableHelper::GetData<FStageTableRow>(Level);
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

FGErrorInfo UStageTableHelper::GetWaveGroupInfo(int32 WaveGroup, TArray<FStageWaveGroupInfo>& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	TArray<TSharedPtr<FStageWaveGroupInfo>>* WaveGroupInfo = Repository->FindWaveGroupInfo(WaveGroup);

	if (!WaveGroupInfo)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FStageWaveGroup find WaveGroupInfo %d"), WaveGroup));
	}

	Result.Empty();
	for (const auto& Info : *WaveGroupInfo) {
		if (Info.IsValid())
		{
			Result.Add(*Info);
		}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetMonsterGroupInfo(int32 MonsterGroup, TArray<FMonsterGroupTableRow>& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	TArray<TSharedPtr<FMonsterGroupTableRow>>* MonsterGroupInfo = Repository->FindMonsterGroupInfo(MonsterGroup);

	if (!MonsterGroupInfo)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FStageMonsterGroupInfo find MonsterGroupInfo %d"), MonsterGroup));
	}

	Result.Empty();
	for (const auto& Info : *MonsterGroupInfo) {
		if (Info.IsValid())
		{
			Result.Add(*Info);
		}
	}

	return FGErrorInfo();
}