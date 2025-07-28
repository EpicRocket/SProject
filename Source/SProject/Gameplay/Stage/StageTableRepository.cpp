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
#include "Gameplay/Stage/Stage.h"
#include "Gameplay/Stage/Types/GameplayStageTypes.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"
#include "Gameplay/Stage/Unit/StageMonsterUnit.h"
#include "Gameplay/Stage/Unit/StageUnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageTableRepository)

//////////////////////////////////////////////////////////////////////////
// UStageTableRepository
//////////////////////////////////////////////////////////////////////////
UStageTableRepository& UStageTableRepository::Get()
{
	if (GEngine == nullptr)
	{
		UE_LOGFMT(LogStage, Error, "[UStageTableRepository::Get()]GEngine is null");
	}
	auto Repo = GEngine->GetEngineSubsystem<UStageTableRepository>();
	return *Repo;
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

	auto StageTableRow = UGTableHelper::GetTableData<FStageTableRow>(StageLevel);
	if (!StageTableRow)
	{
		Promise.EmplaceValue(GameCore::Throw(GameErr::LOAD_FAILURE, TEXT("StageTableRepository")));
		return Future;
	}

	// NOTE. 스테이지 정보
	TWeakObjectPtr<UStageContext> StageContext = StageContexts.FindOrAdd(StageLevel);
	if (StageContext.IsValid())
	{
		Receipt->StageContext = StageContext.Get();
	}
	else
	{
		{
			auto NewContext = NewObject<UStageContext>(this, NAME_None, RF_Public | RF_Transient);
			Receipt->StageContext = NewContext;

			StageContext = StageContexts[StageLevel] = NewContext;
		}
		

		StageContext->StageInfo.Level = StageLevel;
		StageContext->StageInfo.RewardItemKeys = StageTableRow->Reward;
		StageContext->StageInfo.UsePoint = StageTableRow->UsePoint;
		RawAssetList.Emplace(StageTableRow->Icon.ToSoftObjectPath());
		StageContext->StageInfo.IconPtr = StageTableRow->Icon;

		int32 WaveGroup = StageTableRow->WaveGroup;
		auto WaveFilter = [WaveGroup](const FWaveTableRow* Row) -> bool
			{
				return Row->WaveGroup == WaveGroup;
			};

		for (auto WaveTableRow : UGTableHelper::GetTableDatas<FWaveTableRow>().FilterByPredicate(WaveFilter))
		{
			TWeakObjectPtr<UStageWaveContext> WaveContext;

			switch (static_cast<EStageWaveType>(WaveTableRow->Type))
			{
			case EStageWaveType::Default:
				WaveContext = StageWaveContexts.FindOrAdd(WaveTableRow->Index);
				break;

			case EStageWaveType::Repeat:
				WaveContext = RepeatStageWaveContexts.FindOrAdd(WaveTableRow->Index);
				break;

			default:
				continue;
			}

			if (WaveContext.IsValid())
			{
				StageContext->WaveContexts.Emplace(WaveContext.Get());
			}
			else
			{
				{
					auto NewContext = NewObject<UStageWaveContext>(this, NAME_None, RF_Public | RF_Transient);
					StageContext->WaveContexts.Emplace(NewContext);

					WaveContext = NewContext;
					switch (static_cast<EStageWaveType>(WaveTableRow->Type))
					{
					case EStageWaveType::Default:
						StageWaveContexts[WaveTableRow->Index] = NewContext;
						break;

					case EStageWaveType::Repeat:
						RepeatStageWaveContexts[WaveTableRow->Index] = NewContext;
						break;
					}
				}

				WaveContext->WaveInfo.Index = WaveTableRow->Index;
				WaveContext->WaveInfo.DelayTime = FTimespan::FromMilliseconds(WaveTableRow->DelayTime);
				WaveContext->WaveInfo.MonsterGroup = WaveTableRow->MonsterGroup;

				int32 MonsterGroup = WaveTableRow->MonsterGroup;
				auto MonsterGroupFilter = [MonsterGroup](const FMonsterGroupTableRow* Row) -> bool
					{
						return Row->Group == MonsterGroup;
					};

				// NOTE. 몬스터 데이터 추가
				for (auto& MonsterGroupTableRow : UGTableHelper::GetTableDatas<FMonsterGroupTableRow>().FilterByPredicate(MonsterGroupFilter))
				{
					auto MonsterTableRow = UGTableHelper::GetTableData<FMonsterTableRow>(MonsterGroupTableRow->MonsterKind);
					if (!MonsterTableRow)
					{
						continue;
					}

					FStageMonsterGroupInfo MonsterGroupInfo;
					MonsterGroupInfo.Index = MonsterGroupTableRow->Index;
					MonsterGroupInfo.Amount = MonsterGroupTableRow->Amount;
					MonsterGroupInfo.AmountValue = MonsterGroupTableRow->AmountValue;
					MonsterGroupInfo.Position = MonsterGroupTableRow->Position;
					MonsterGroupInfo.AmountDelayTime = FTimespan::FromMilliseconds(MonsterGroupTableRow->AmountDelayTime);

					{
						TWeakObjectPtr<UStageMonsterContext> Context = MonsterContexts.FindOrAdd(MonsterTableRow->Index);
						if (Context.IsValid())
						{
							MonsterGroupInfo.MonsterContext = Context.Get();
						}
						else
						{
							MonsterGroupInfo.MonsterContext = NewObject<UStageMonsterContext>(this, NAME_None, RF_Public | RF_Transient);
							Context = MonsterContexts[MonsterTableRow->Index] = MonsterGroupInfo.MonsterContext;

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

					WaveContext->MonsterGroupInfos.Emplace(MonsterGroupInfo);
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
					auto TowerTableFilter = [Kind](const FNormalTowerTableRow* Row) -> bool
						{
							return Row->Kind == Kind;
						};

					for (const auto& TowerTableRow : UGTableHelper::GetTableDatas<FNormalTowerTableRow>().FilterByPredicate(TowerTableFilter))
					{
						TWeakObjectPtr<UStageTowerContext> Context = NormalTowerContexts.FindOrAdd(MakeTuple(TowerTableRow->Kind, TowerTableRow->Level));
						if (Context.IsValid())
						{
							StageContext->TowerContexts.Emplace(Context.Get());
						}
						else
						{
							auto NewContext = NewObject<UStageTowerContext>(this, NAME_None, RF_Public | RF_Transient);
							StageContext->TowerContexts.Emplace(NewContext);

							Context = NormalTowerContexts[MakeTuple(TowerTableRow->Kind, TowerTableRow->Level)] = NewContext;

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

TWeakObjectPtr<UStageContext> UStageTableRepository::FindStageContext(int32 Key)
{
	auto ContextIter = StageContexts.Find(Key);
	if (ContextIter && ContextIter->IsValid())
	{
		return *ContextIter;
	}
	UE_LOGFMT(LogStage, Warning, "StageContext를 찾을 수 없습니다. [Key:{0}]", Key);
	return TWeakObjectPtr<UStageContext>{};
}

TWeakObjectPtr<UStageMonsterContext> UStageTableRepository::FindNormalMonsterContext(int32 Key)
{
	auto ContextIter = MonsterContexts.Find(Key);
	if (ContextIter && ContextIter->IsValid())
	{
		return *ContextIter;
	}
	UE_LOGFMT(LogStage, Warning, "MonsterContext를 찾을 수 없습니다. [Key:{0}]", Key);
	return TWeakObjectPtr<UStageMonsterContext>{};
}

TWeakObjectPtr<UStageTowerContext> UStageTableRepository::FindNormalTowerContext(int32 Kind, int32 Level)
{
	auto ContextIter = NormalTowerContexts.Find(MakeTuple(Kind, Level));
	if (ContextIter && ContextIter->IsValid())
	{
		return *ContextIter;
	}
	UE_LOGFMT(LogStage, Warning, "NormalTowerContext를 찾을 수 없습니다. [Kind:{0}][Level:{1}]", Kind, Level);
	return TWeakObjectPtr<UStageTowerContext>{};
}

TOptional<int32> UStageTableRepository::FindNormalTowerMaxLevel(int32 Kind)
{
	auto TowerTableFilter = [Kind](const FNormalTowerTableRow* Row) -> bool
		{
			return Row->Kind == Kind;
		};

	TOptional<int32> MaxLevel = 0;
	for (const auto& TableRow : UGTableHelper::GetTableDatas<FNormalTowerTableRow>().FilterByPredicate(TowerTableFilter))
	{
		if (TableRow->Level > *MaxLevel)
		{
			MaxLevel = TableRow->Level;
		}
	}

	return MaxLevel;
}

//////////////////////////////////////////////////////////////////////////
// UStageTableHelper
//////////////////////////////////////////////////////////////////////////
FGErrorInfo UStageTableHelper::GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
{
	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto Context = UStageTableRepository::Get().FindNormalTowerContext(Kind, Level);
		if (!Context.IsValid())
		{
			auto Msg = FString::Printf(TEXT("GetBuildStageTower(TowerType:%s, Kind:%d, Level:%d, Result)"), *UEnum::GetValueAsString(TowerType), Kind, Level);
			return GameCore::Throw(GameErr::POINTER_INVALID, Msg);
		}

		Result = Context->TowerInfo;
	} break;

	default: {
		auto Msg = FString::Printf(TEXT("GetBuildStageTower(TowerType:%s, Kind:%d, Level:%d, Result)"), *UEnum::GetValueAsString(TowerType), Kind, Level);
		return GameCore::Throw(GameErr::ENUM_INVALID, Msg);
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result)
{
	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto MaxLevel = UStageTableRepository::Get().FindNormalTowerMaxLevel(Kind);
		if (!MaxLevel.IsSet())
		{
			auto Msg = FString::Printf(TEXT("GetNextStageTower(TowerType:%s, Kind:%d, Level:%d, Result)"), *UEnum::GetValueAsString(TowerType), Kind, Level);
			return GameCore::Throw(GameErr::VALUE_INVALID, Msg);
		}

		if (Level >= *MaxLevel)
		{
			auto Msg = FString::Printf(TEXT("GetNextStageTower(TowerType:%s, Kind:%d, Level:%d, Result):Level이 MaxLevel보다 큽니다.[MaxLevel:%d]"), *UEnum::GetValueAsString(TowerType), Kind, Level, *MaxLevel);
			return GameCore::Throw(GameErr::VALUE_INVALID, Msg);
		}

		auto Context = UStageTableRepository::Get().FindNormalTowerContext(Kind, Level + 1);
		if (!Context.IsValid())
		{
			auto Msg = FString::Printf(TEXT("GetNextStageTower(TowerType:%s, Kind:%d, Level:%d, Result)"), *UEnum::GetValueAsString(TowerType), Kind, Level + 1);
			return GameCore::Throw(GameErr::POINTER_INVALID, Msg);
		}

		Result = Context->TowerInfo;
	} break;

	default: {
		auto Msg = FString::Printf(TEXT("GetNextStageTower(TowerType:%s, Kind:%d, Level:%d, Result)"), *UEnum::GetValueAsString(TowerType), Kind, Level);
		return GameCore::Throw(GameErr::ENUM_INVALID, Msg);
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageTowerMaxLevel(EStageTowerType TowerType, int32 Kind, int32& Result)
{
	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto MaxLevel = UStageTableRepository::Get().FindNormalTowerMaxLevel(Kind);
		if (!MaxLevel.IsSet())
		{
			auto Msg = FString::Printf(TEXT("GetStageTowerMaxLevel(TowerType:%s, Kind:%d, Result)"), *UEnum::GetValueAsString(TowerType), Kind);
			return GameCore::Throw(GameErr::VALUE_INVALID, Msg);
		}
		Result = *MaxLevel;

	} break;

	default: {
		auto Msg = FString::Printf(TEXT("GetStageTowerMaxLevel(TowerType:%s, Kind:%d, Result)"), *UEnum::GetValueAsString(TowerType), Kind);
		return GameCore::Throw(GameErr::ENUM_INVALID, Msg);
	}
	}

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageTowerBaseStats(EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result)
{
	switch (TowerType)
	{
	case EStageTowerType::Normal: {
		auto Context = UStageTableRepository::Get().FindNormalTowerContext(Kind, Level);
		if (!Context.IsValid())
		{
			auto Msg = FString::Printf(TEXT("GetStageTowerBaseStats(TowerType:%s, Kind:%d, Level:%d, Result)"), *UEnum::GetValueAsString(TowerType), Kind, Level);
			return GameCore::Throw(GameErr::POINTER_INVALID, Msg);
		}

		auto TowerRow = UGTableHelper::GetTableData<FNormalTowerTableRow>(Context->TowerInfo.Index);
		if (!TowerRow)
		{
			auto Msg = FString::Printf(TEXT("GetStageTowerBaseStats(TowerType:%s, Kind:%d, Level:%d, Result)"), *UEnum::GetValueAsString(TowerType), Kind, Level);
			return GameCore::Throw(GameErr::TABLE_INVALID, Msg);
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

FGErrorInfo UStageTableHelper::GetStageMonsterInfo(int32 MonsterKey, FStageMonsterInfo& Result)
{
	auto Context = UStageTableRepository::Get().FindNormalMonsterContext(MonsterKey);
	if (!Context.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("GetStageWaveContexts(MonsterKey:%d, Contexts)"), MonsterKey));
	}

	Result = Context->MonsterInfo;
	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageMonsterBaseStats(int32 MonsterKey, TMap<EStageUnitAttribute, double>& Result)
{
	auto Row = UGTableHelper::GetTableData<FMonsterTableRow>(MonsterKey);
	if (!Row)
	{
		return GameCore::Throw(GameErr::TABLE_INVALID, FString::Printf(TEXT("GetStageWaveContexts(MonsterKey:%d, Result)"), MonsterKey));
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

FGErrorInfo UStageTableHelper::GetStageLastWave(int32 StageLevel, int32& Result)
{
	auto Context = UStageTableRepository::Get().FindStageContext(StageLevel);
	if (!Context.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("GetStageLastWave(StageLevel:%d, Result)"), StageLevel));
	}

	Result = Context->WaveContexts.Num() - 1;

	return GameCore::Pass();
}

FGErrorInfo UStageTableHelper::GetStageWaveContexts(int32 StageLevel, TArray<UStageWaveContext*>& Result)
{
	auto Context = UStageTableRepository::Get().FindStageContext(StageLevel);
	if (!Context.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("GetStageWaveContexts(StageLevel:%d, Result)"), StageLevel));
	}

	Result = Context->WaveContexts;

	return GameCore::Pass();
}
