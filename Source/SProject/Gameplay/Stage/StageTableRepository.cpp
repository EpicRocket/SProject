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
		UE_LOGFMT(LogStage, Error, "[UStageTableRepository] World를 찾을 수 없습니다.");
		return nullptr;
	}

	auto Repo = UGameInstance::GetSubsystem<UStageTableRepository>(World->GetGameInstance());
	if (!Repo)
	{
		UE_LOGFMT(LogStage, Error, "[UStageTableRepository] Repository를 찾을 수 없습니다. [참조 오브젝트:{WorldContextObject}]", *WorldContextObject->GetName());
		return nullptr;
	}

	return Repo;
}

TFuture<FGErrorInfo> UStageTableRepository::Load(UStageTableReceipt*& Receipt, int32 StageLevel, TMap<EStageTowerType, TSet<int32>> TowerList)
{
	TPromise<FGErrorInfo> Promise;
	auto Future = Promise.GetFuture();

	if (!IsValid(Receipt))
	{
		Promise.EmplaceValue(GameCore::Throw(GameErr::POINTER_INVALID, TEXT("Receipt")));
		return Future;
	}

	// Streamable List
	TSet<FSoftObjectPath> RawAssetList;

	if (auto Stage = UGTableHelper::GetTableData<FStageTableRow>(StageLevel))
	{
		int32 WaveGroup = Stage->WaveGroup;
		auto WaveFilter = [WaveGroup](const FWaveTableRow* Row) -> bool
			{
				return Row->WaveGroup == WaveGroup;
			};

		for (auto& WaveTableRow : UGTableHelper::GetTableDatas<FWaveTableRow>().FilterByPredicate(WaveFilter))
		{
			int32 MonsterGroup = WaveTableRow->MonsterGroup;
			auto MonsterGroupFilter = [MonsterGroup](const FMonsterGroupTableRow* Row) -> bool
				{
					return Row->Index == MonsterGroup;
				};

			// NOTE. 몬스터 데이터 추가
			for (auto& MonsterGroupTableRow : UGTableHelper::GetTableDatas<FMonsterGroupTableRow>().FilterByPredicate(MonsterGroupFilter))
			{
				auto MonsterTableRow = UGTableHelper::GetTableData<FMonsterTableRow>(MonsterGroupTableRow->MonsterKind);
				if (!MonsterTableRow)
				{
					continue;
				}

				TWeakObjectPtr<UStageMonsterContext> Context = MonsterContexts.FindOrAdd(MonsterTableRow->Index);
				if (Context.IsValid())
				{
					Receipt->Contexts.Emplace(Context.Get());
				}
				else
				{
					auto NewContext = NewObject<UStageMonsterContext>(this, NAME_None, RF_Public | RF_Transient);
					Context = NewContext;
					Receipt->Contexts.Emplace(NewContext);

					Context->MonsterInfo.Index = MonsterTableRow->Index;
					Context->MonsterInfo.Level = MonsterTableRow->Level;
					Context->MonsterInfo.Grade = MonsterTableRow->Grade;
					Context->MonsterInfo.Name = MonsterTableRow->Name;
					Context->MonsterInfo.AttackType = MonsterTableRow->AttackType;
					Context->MonsterInfo.UnitClassPtr = MonsterTableRow->Unit;
					RawAssetList.Emplace(MonsterTableRow->Unit.ToSoftObjectPath());
					Context->MonsterInfo.IconPtr = MonsterTableRow->Icon;
					RawAssetList.Emplace(MonsterTableRow->Icon.ToSoftObjectPath());
					Context->MonsterInfo.AIPtr = MonsterTableRow->AI;
					RawAssetList.Emplace(MonsterTableRow->AI.ToSoftObjectPath());
				}
			}
		}
	}

	for (auto& [Type, Towers] : TowerList)
	{
		switch (Type)
		{
		case EStageTowerType::Normal:
		{
			for (int32 Kind : Towers)
			{
				auto TowerTableRow = UGTableHelper::GetTableData<FNormalTowerTableRow>(Kind);
				if (!TowerTableRow)
				{
					continue;
				}

				TWeakObjectPtr<UStageTowerContext> Context = NormalTowerContexts.FindOrAdd(MakeTuple(TowerTableRow->Kind, TowerTableRow->Level));
				if (Context.IsValid())
				{
					Receipt->Contexts.Emplace(Context.Get());
				}
				else
				{
					auto NewContext = NewObject<UStageTowerContext>(this, NAME_None, RF_Public | RF_Transient);
					Context = NewContext;
					Receipt->Contexts.Emplace(Context.Get());

					Context->TowerInfo.TowerType = Type;
					Context->TowerInfo.Index = TowerTableRow->Index;
					Context->TowerInfo.Kind = TowerTableRow->Kind;
					Context->TowerInfo.Level = TowerTableRow->Level;
					Context->TowerInfo.UsePoint = TowerTableRow->UsePoint;
					Context->TowerInfo.Name = TowerTableRow->Name;
					Context->TowerInfo.AttackType = TowerTableRow->AttackType;
					Context->TowerInfo.UnitClassPtr = TowerTableRow->Unit;
					RawAssetList.Emplace(TowerTableRow->Unit.ToSoftObjectPath());
					Context->TowerInfo.IconPtr = TowerTableRow->Icon;
					RawAssetList.Emplace(TowerTableRow->Icon.ToSoftObjectPath());
					Context->TowerInfo.AIPtr = TowerTableRow->AI;
					RawAssetList.Emplace(TowerTableRow->AI.ToSoftObjectPath());
				}
			}
		}
		}
	}

	// Load Assets
	RequestTasks(
		RawAssetList.Array(),
		[this
		, ThisPtr = TWeakObjectPtr<UStageTableRepository>(this)
		, Promise = MoveTemp(Promise)
		]() mutable
		{
			if (!ThisPtr.IsValid())
			{
				Promise.EmplaceValue(GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UStageTableRepository")));
				return;
			}

			Promise.EmplaceValue(GameCore::Pass());
		}
	);

	return Future;
}

TWeakObjectPtr<UStageTowerContext> UStageTableRepository::FindNormalTowerContext(int32 Kind, int32 Level) const
{
	/*auto Iter = NormalTowerByCompositeKey.Find(MakeTuple(Kind, Level));
	if (Iter && Iter->IsValid())
	{
		return *Iter;
	}*/
	return TWeakObjectPtr<UStageTowerContext>{};
}

TOptional<int32> UStageTableRepository::FindNormalTowerMaxLevel(int32 Kind) const
{
	/*auto Iter = NormalTowerMaxLevels.Find(Kind);
	if (Iter)
	{
		return *Iter;
	}*/
	return TOptional<int32>{};
}

TWeakObjectPtr<UStageMonsterContext> UStageTableRepository::FindNormalMonsterContext(int32 Key) const
{
	/*auto Iter = MonsterContexts.Find(Key);
	if (Iter && IsValid((*Iter)))
	{
		return *Iter;
	}*/
	return TWeakObjectPtr<UStageMonsterContext>{};
}

//////////////////////////////////////////////////////////////////////////
// UStageTableHelper
//////////////////////////////////////////////////////////////////////////
FGErrorInfo UStageTableHelper::GetBuildStageTower(const UObject* WorldContextObject, EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
{
	auto Repository = UStageTableRepository::Get(WorldContextObject);
	if (!Repository)
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
	if (!Repository)
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
	if (!Repository)
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
	if (!Repository)
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
		Result.FindOrAdd(EStageUnitAttribute::Defence) = TowerRow->Defense;
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
	if (!Repository)
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
	if (!Repository)
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
	Result.FindOrAdd(EStageUnitAttribute::Defence) = Row->Defense;
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
