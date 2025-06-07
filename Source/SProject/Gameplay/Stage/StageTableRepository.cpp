// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageTableRepository.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
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
UStageTableRepository* UStageTableRepository::Get(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}
	return UGameInstance::GetSubsystem<UStageTableRepository>(World->GetGameInstance());
}

void UStageTableRepository::OnLoad()
{
	SCOPED_BOOT_TIMING("Active StageTableRepository Load");

	EObjectFlags Flags = RF_Public | RF_Transient;

	// NOTE. 타워
	{
		NormalTowerContexts.Empty();
		auto NormalTowerTableRows = UGTableHelper::GetTableDatas<FNormalTowerTableRow>();
		for (FNormalTowerTableRow* Row : NormalTowerTableRows)
		{
			auto Context = NewObject<UStageTowerContext>(this, NAME_None, Flags);
			NormalTowerContexts.Emplace(Row->Index, Context);
			NormalTowerByCompositeKey.Emplace(MakeTuple(Row->Kind, Row->Level), Context);
			auto& MaxLevel = NormalTowerMaxLevels.FindOrAdd(Row->Kind);
			MaxLevel = FMath::Max(MaxLevel, Row->Level);

			Context->TowerInfo.TowerType = EStageTowerType::Normal;
			Context->TowerInfo.Index = Row->Index;
			Context->TowerInfo.Kind = Row->Kind;
			Context->TowerInfo.Level = Row->Level;
			Context->TowerInfo.UsePoint = Row->UsePoint;
			Context->TowerInfo.Name = Row->Name;
			Context->TowerInfo.AttackType = Row->AttackType;
			RequestTask(
				Row->Unit.ToSoftObjectPath(),
				[this, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this), WeakContext = TWeakObjectPtr<UStageTowerContext>(Context), Asset = Row->Unit]()
				{
					if (ThisPtr.IsValid())
					{
						if (WeakContext.IsValid())
						{
							WeakContext->TowerInfo.UnitClass = Asset.Get();
						}
					}
				}
				);
			RequestTask(
				Row->Icon.ToSoftObjectPath(),
				[this, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this), WeakContext = TWeakObjectPtr<UStageTowerContext>(Context), Asset = Row->Icon]()
				{
					if (ThisPtr.IsValid())
					{
						if (WeakContext.IsValid())
						{
							WeakContext->TowerInfo.Icon = Asset.Get();
						}
					}
				}
			);
			RequestTask(
				Row->AI.ToSoftObjectPath(),
				[this, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this), WeakContext = TWeakObjectPtr<UStageTowerContext>(Context), Asset = Row->AI]()
				{
					if (ThisPtr.IsValid())
					{
						if (WeakContext.IsValid())
						{
							WeakContext->TowerInfo.AI = Asset.Get();
						}
					}
				}
			);
		}
	}

	// NOTE. Monster
	{
		MonsterContexts.Empty();
		auto MonsterTableRows = UGTableHelper::GetTableDatas<FMonsterTableRow>();
		for (FMonsterTableRow* Row : MonsterTableRows)
		{
			auto Context = NewObject<UStageMonsterContext>(this, NAME_None, Flags);
			MonsterContexts.Emplace(Row->Index, Context);

			Context->MonsterInfo.Index = Row->Index;
			Context->MonsterInfo.Level = Row->Level;
			Context->MonsterInfo.Grade = Row->Grade;
			Context->MonsterInfo.Name = Row->Name;
			Context->MonsterInfo.AttackType = Row->AttackType;
			RequestTask(
				Row->Unit.ToSoftObjectPath(),
				[this, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this), WeakContext = TWeakObjectPtr<UStageMonsterContext>(Context), Asset = Row->Unit]()
				{
					if (ThisPtr.IsValid())
					{
						if (WeakContext.IsValid())
						{
							WeakContext->MonsterInfo.UnitClass = Asset.Get();
						}
					}
				}
			);

			RequestTask(
				Row->Icon.ToSoftObjectPath(),
				[this, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this), WeakContext = TWeakObjectPtr<UStageMonsterContext>(Context), Asset = Row->Icon]()
				{
					if (ThisPtr.IsValid())
					{
						if (WeakContext.IsValid())
						{
							WeakContext->MonsterInfo.Icon = Asset.Get();
						}
					}
				}
			);

			RequestTask(
				Row->AI.ToSoftObjectPath(),
				[this, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this), WeakContext = TWeakObjectPtr<UStageMonsterContext>(Context), Asset = Row->AI]()
				{
					if (ThisPtr.IsValid())
					{
						if (WeakContext.IsValid())
						{
							WeakContext->MonsterInfo.AI = Asset.Get();
						}
					}
				}
			);
		}
	}
}

void UStageTableRepository::OnUnload()
{
	NormalTowerContexts.Empty();
	NormalTowerByCompositeKey.Empty();
	NormalTowerMaxLevels.Empty();

	UE_LOGFMT(LogStage, Log, "StageTableRepository Unload Complete");
}

TWeakObjectPtr<UStageTowerContext> UStageTableRepository::FindNormalTowerContext(int32 Kind, int32 Level) const
{
	auto Iter = NormalTowerByCompositeKey.Find(MakeTuple(Kind, Level));
	if (Iter && Iter->IsValid())
	{
		return *Iter;
	}
	return TWeakObjectPtr<UStageTowerContext>{};
}

TOptional<int32> UStageTableRepository::FindNormalTowerMaxLevel(int32 Kind) const
{
	auto Iter = NormalTowerMaxLevels.Find(Kind);
	if (Iter)
	{
		return *Iter;
	}
	return TOptional<int32>{};
}

TWeakObjectPtr<UStageMonsterContext> UStageTableRepository::FindNormalMonsterContext(int32 Key) const
{
	auto Iter = MonsterContexts.Find(Key);
	if (Iter && IsValid((*Iter)))
	{
		return *Iter;
	}
	return TWeakObjectPtr<UStageMonsterContext>{};
}

//////////////////////////////////////////////////////////////////////////
// UStageTableHelper
//////////////////////////////////////////////////////////////////////////
FGErrorInfo UStageTableHelper::GetBuildStageTower(const UObject* WorldContextObject, EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
{
	auto Repository = UStageTableRepository::Get(WorldContextObject);
	if (ensure(Repository))
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID);
	}

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto Context = Repository->FindNormalTowerContext(Kind, Level);
		if (!Context.IsValid())
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("데이터를 찾을 수 없습니다: TowerType:%s, Kind:%s, Level:%s"), *UEnum::GetValueAsString(TowerType), Kind, Level));
		}

		Result = Context->TowerInfo;
	}
	break;

	default: {
		return GameCore::Throw(GameErr::ENUM_INVALID, FString::Printf(TEXT("TowerType:%s"), *UEnum::GetValueAsString(TowerType)));
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetNextStageTower(const UObject* WorldContextObject, EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
{
	auto Repository = UStageTableRepository::Get(WorldContextObject);
	if (ensure(Repository))
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID);
	}

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto MaxLevel = Repository->FindNormalTowerMaxLevel(Kind);
		if (!MaxLevel.IsSet())
		{
			return GameCore::Throw(GameErr::VALUE_INVALID, FString::Printf(TEXT("MaxLevel를 찾을 수 없습니다: TowerType:%s, Kind:%d"), *UEnum::GetValueAsString(TowerType), Kind));
		}

		if (Level >= *MaxLevel)
		{
			return GameCore::Throw(GameErr::VALUE_INVALID, FString::Printf(TEXT("Level이 MaxLevel보다 큽니다: TowerType:%s, Level:%d, MaxLevel:%d"), *UEnum::GetValueAsString(TowerType), Level, *MaxLevel));
		}

		auto Context = Repository->FindNormalTowerContext(Kind, Level + 1);
		if (!Context.IsValid())
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("데이터를 찾을 수 없습니다: TowerType:%s, Kind:%d, Level:%d not found"), *UEnum::GetValueAsString(TowerType), Kind, Level + 1));
		}

		Result = Context->TowerInfo;
	}
	break;

	default: {
		return GameCore::Throw(GameErr::ENUM_INVALID, FString::Printf(TEXT("TowerType:%s"), *UEnum::GetValueAsString(TowerType)));
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageTowerMaxLevel(const UObject* WorldContextObject, EStageTowerType TowerType, int32 Kind, int32& Result)
{
	auto Repository = UStageTableRepository::Get(WorldContextObject);
	if (ensure(Repository))
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID);
	}

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto MaxLevel = Repository->FindNormalTowerMaxLevel(Kind);
		if (!MaxLevel.IsSet())
		{
			return GameCore::Throw(GameErr::VALUE_INVALID, FString::Printf(TEXT("MaxLevel를 찾을 수 없습니다: TowerType:%s, Kind:%d"), *UEnum::GetValueAsString(TowerType), Kind));
		}
		Result = *MaxLevel;

	} break;

	default: {
		return GameCore::Throw(GameErr::ENUM_INVALID, FString::Printf(TEXT("TowerType:%s"), *UEnum::GetValueAsString(TowerType)));
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageTowerBaseStats(const UObject* WorldContextObject, EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result)
{
	auto Repository = UStageTableRepository::Get(WorldContextObject);
	if (ensure(Repository))
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID);
	}

	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto Context = Repository->FindNormalTowerContext(Kind, Level);
		if (!Context.IsValid())
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("데이터를 찾을 수 없습니다: TowerType:%s, Kind:%s, Level:%s"), *UEnum::GetValueAsString(TowerType), Kind, Level));
		}

		auto TowerRow = UGTableHelper::GetTableData<FNormalTowerTableRow>(Context->TowerInfo.Index);
		if (!TowerRow)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FNormalTowerTableRow find Index %d"), Context->TowerInfo.Index));
		}

		Result.Empty();
		Result.FindOrAdd(EStageUnitAttribute::Level) = TowerRow->Level;
		Result.FindOrAdd(EStageUnitAttribute::Grade) = TowerRow->Grade;
		Result.FindOrAdd(EStageUnitAttribute::Attack) = TowerRow->Attack;
		Result.FindOrAdd(EStageUnitAttribute::Defence) = TowerRow->Defence;
		Result.FindOrAdd(EStageUnitAttribute::MaxHp) = TowerRow->Hp;
		Result.FindOrAdd(EStageUnitAttribute::Hp) = TowerRow->Hp;
		Result.FindOrAdd(EStageUnitAttribute::AttackSpeed) = TowerRow->AttackSpeed;
		Result.FindOrAdd(EStageUnitAttribute::MovementSpeed) = 0.0;
		Result.FindOrAdd(EStageUnitAttribute::Range) = TowerRow->Range;
		Result.FindOrAdd(EStageUnitAttribute::SplashScale) = TowerRow->SplashScale;

	} break;

	default: {
		return GameCore::Throw(GameErr::ENUM_INVALID, UEnum::GetValueAsString(TowerType));
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMonsterInfo(const UObject* WorldContextObject, int32 MonsterKey, FStageMonsterInfo& Result)
{
	auto Repository = UStageTableRepository::Get(WorldContextObject);
	if (ensure(Repository))
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID);
	}

	auto Context = Repository->FindNormalMonsterContext(MonsterKey);
	if (!Context.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("데이터를 찾을 수 없습니다: MonsterKey:%d"), MonsterKey));
	}

	Result = Context->MonsterInfo;

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMonsterBaseStats(const UObject* WorldContextObject, int32 MonsterKey, TMap<EStageUnitAttribute, double>& Result)
{
	auto Repository = UStageTableRepository::Get(WorldContextObject);
	if (ensure(Repository))
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID);
	}

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

FGErrorInfo UStageTableHelper::GetWaveGroupInfo(const UObject* WorldContextObject, int32 WaveGroup, TArray<FStageWaveGroupInfo>& Result)
{
	/*auto Repository = UStageTableRepository::Get();
	check(Repository);*/

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

FGErrorInfo UStageTableHelper::GetMonsterGroupInfo(const UObject* WorldContextObject, int32 MonsterGroup, TArray<FMonsterGroupTableRow>& Result)
{
	/*auto Repository = UStageTableRepository::Get();
	check(Repository);*/

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
