// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Table/Subsystem/GTableRepositorySubsystem.h"
#include "Templates/SharedPointer.h"
#include "Async/Future.h"

#include "StageTableRepository.generated.h"

struct FGErrorInfo;
class UWorld;

enum class EStageUnitAttribute : uint8;

// Stage forward declare
class UStageContext;
class UStageWaveContext;

// Tower forward declare
enum class EStageTowerType : uint8;
struct FStageTowerInfo;
class AStageTowerUnit;
class UStageTowerContext;

// Monster forward declare
struct FStageMonsterInfo;
class UStageMonsterContext;

UCLASS()
class UStageTableReceipt : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UStageContext> StageContext;

};

UCLASS()
class MY_API UStageTableRepository : public UGTableRepositorySubsystem
{
	GENERATED_BODY()

	friend class UStageTableHelper;
public:
	static UStageTableRepository& Get();

	TFuture<FGErrorInfo> Load(UStageTableReceipt*& Receipt, int32 StageLevel, TMap<EStageTowerType, TSet<int32>> TowerList);

private:
	using TableKey = int32;
	using KindType = int32;
	using LevelType = int32;

	// NOTE. 스테이지
	TMap<TableKey, TWeakObjectPtr<UStageContext>> StageContexts;
	TMap<TableKey, TWeakObjectPtr<UStageWaveContext>> StageWaveContexts;
	TMap<TableKey, TWeakObjectPtr<UStageWaveContext>> RepeatStageWaveContexts;
	TWeakObjectPtr<UStageContext> FindStageContext(int32 Key);

	// NOTE. 몬스터
	TMap<TableKey, TWeakObjectPtr<UStageMonsterContext>> MonsterContexts;
	TWeakObjectPtr<UStageMonsterContext> FindNormalMonsterContext(int32 Key);

	// Note. 타워
	using TowerKeyType = TTuple<KindType, LevelType>;
	TMap<TowerKeyType, TWeakObjectPtr<UStageTowerContext>> NormalTowerContexts;
	TWeakObjectPtr<UStageTowerContext> FindNormalTowerContext(int32 Kind, int32 Level);
	TOptional<int32> FindNormalTowerMaxLevel(int32 Kind);

};

UCLASS()
class MY_API UStageTableHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetStageTowerMaxLevel(EStageTowerType TowerType, int32 Kind, int32& Result);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetStageTowerBaseStats(EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetStageMonsterInfo(int32 MonsterKey, FStageMonsterInfo& Result);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetStageMonsterBaseStats(int32 MonsterKey, TMap<EStageUnitAttribute, double>& Result);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetStageLastWave(int32 StageLevel, int32& Result);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetStageWaveContexts(int32 StageLevel, TArray<UStageWaveContext*>& Result);

};
