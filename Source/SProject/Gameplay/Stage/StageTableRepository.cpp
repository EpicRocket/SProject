
#include "StageTableRepository.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
#include "BehaviorTree/BehaviorTree.h"
// include GameCore
#include "Error/GError.h"
#include "Table/GTableHelper.h"
// include Project
#include "Table/TowerTable.h"
#include "Table/MonsterTable.h"
#include "Table/StageTable.h"
#include "Table/MonsterGroupTable.h"
#include "Gameplay/Stage/StageLogging.h"
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

void UStageTableRepository::OnLoad()
{
	SCOPED_BOOT_TIMING("Active StageTableRepository Load");

	
	{
		NormalTowerInfos.Empty();
		auto NormalTowerTableRows = UGTableHelper::GetTableDatas<FNormalTowerTableRow>();
		for (auto& Row : NormalTowerTableRows)
		{
			TSharedPtr<FStageTowerInfo> NewData = MakeShared<FStageTowerInfo>();
			NormalTowerInfos.Emplace(Row->Index, NewData);

			NewData->TowerType = EStageTowerType::Normal;
			NewData->Index = Row->Index;
			NewData->Kind = Row->Kind;
			NewData->Level = Row->Level;
			NewData->UsePoint = Row->UsePoint;
			NewData->Name = Row->Name;
			NewData->AttackType = Row->AttackType;
			RequestTask(
				Row->Unit.ToSoftObjectPath(),
				[this, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this), AffectedData = TWeakPtr<FStageTowerInfo>(NewData), Asset = Row->Unit]()
				{
					if (ThisPtr.IsValid())
					{
						if (AffectedData.IsValid())
						{
							AffectedData.Pin()->UnitClass = Asset.Get();
						}
					}
				}
				);
			RequestTask(
				Row->Icon.ToSoftObjectPath(),
				[this, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this), AffectedData = TWeakPtr<FStageTowerInfo>(NewData), Asset = Row->Icon]()
				{
					if (ThisPtr.IsValid())
					{
						if (AffectedData.IsValid())
						{
							AffectedData.Pin()->Icon = Asset.Get();
						}
					}
				}
			);
			RequestTask(
				Row->AI.ToSoftObjectPath(),
				[this, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this), AffectedData = TWeakPtr<FStageTowerInfo>(NewData), Asset = Row->AI]()
				{
					if (ThisPtr.IsValid())
					{
						if (AffectedData.IsValid())
						{
							AffectedData.Pin()->AI = Asset.Get();
						}
					}
				}
			);
		}
	}




	UE_LOGFMT(LogStage, Verbose, "StageTableRepository Load Complete");
}

void UStageTableRepository::OnUnload()
{
	NormalTowerInfos.Empty();

	UE_LOGFMT(LogStage, Verbose, "StageTableRepository Unload Complete");
}

//////////////////////////////////////////////////////////////////////////
// UStageTableHelper
//////////////////////////////////////////////////////////////////////////
FGErrorInfo UStageTableHelper::GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	/*switch (TowerType)
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
	}*/

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	/*int32 MaxLevel = GetStageTowerMaxLevel(TowerType, Kind);
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
	}*/

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageTowerSellPrice(EStageTowerType TowerType, int32 Kind, int32 Level, int64& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	/*switch (TowerType)
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
	}*/

	return GameCore::Pass();
}

int32 UStageTableHelper::GetStageTowerMaxLevel(EStageTowerType TowerType, int32 Kind)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	int32 Result = 0;

	/*switch (TowerType)
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
		return 0;
	}
	}*/

	return FMath::Max(0, Result);
}

FGErrorInfo UStageTableHelper::GetStageTowerBaseStats(EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	/*switch (TowerType)
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
	}*/

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMonsterInfo(int32 MonsterKey, FStageMonsterInfo& Result)
{
	auto Repo = UStageTableRepository::Get();
	check(Repo);

	/*TSharedPtr<FStageMonsterInfo> MonsterInfo = Repo->FindMonsterInfo(MonsterKey);
	if (!MonsterInfo.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FStageMonsterInfo find MonsterKey %d"), MonsterKey));
	}

	Result = *MonsterInfo;*/

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMonsterBaseStats(int32 MonsterKey, TMap<EStageUnitAttribute, double>& Result)
{
	auto Row = UGTableHelper::GetTableData<FMonsterTableRow>(MonsterKey);
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
	auto Row = UGTableHelper::GetTableData<FStageTableRow>(Level);
	if (!Row)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FStageTableRow find Level %d"), Level));
	}
	Result = *Row;
	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMap(int32 Level, TSoftObjectPtr<UWorld>& Map)
{
	auto Row = UGTableHelper::GetTableData<FStageTableRow>(Level);
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

	/*TArray<TSharedPtr<FStageWaveGroupInfo>>* WaveGroupInfo = Repository->FindWaveGroupInfo(WaveGroup);

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
	}*/

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetMonsterGroupInfo(int32 MonsterGroup, TArray<FMonsterGroupTableRow>& Result)
{
	auto Repository = UStageTableRepository::Get();
	check(Repository);

	/*TArray<TSharedPtr<FMonsterGroupTableRow>>* MonsterGroupInfo = Repository->FindMonsterGroupInfo(MonsterGroup);

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
	}*/

	return FGErrorInfo();
}
