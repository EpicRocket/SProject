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

// Tower forward declare
enum class EStageTowerType : uint8;
struct FStageTowerInfo;
class AStageTowerUnit;
class UStageTowerContext;

// Monster forward declare
struct FStageMonsterInfo;
class UStageMonsterContext;

struct FStageMonsterGroupDetail;
struct FStageMonsterGroupInfo;
struct FStageWaveGroupInfo;

UCLASS()
class UStageTableReceipt : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UGContext*> Contexts;

};

UCLASS()
class MY_API UStageTableRepository : public UGTableRepositorySubsystem
{
	GENERATED_BODY()

	friend class UStageTableHelper;
public:
	static UStageTableRepository* Get(const UObject* WorldContextObject);

	TFuture<FGErrorInfo> Load(UStageTableReceipt*& Receipt, int32 StageLevel, TMap<EStageTowerType, TSet<int32>> TowerList);

private:
	using TableKey = int32;
	using KindType = int32;
	using LevelType = int32;
	using MonsterGroupType = int32;

	using TowerKeyType = TTuple<KindType, LevelType>;
	TMap<TowerKeyType, TWeakObjectPtr<UStageTowerContext>> NormalTowerContexts;

	TMap<TableKey, TWeakObjectPtr<UStageMonsterContext>> MonsterContexts;

	TArray<FStageWaveGroupInfo> StageWaveGroup;
	TMap<MonsterGroupType, TArray<FMonsterGroupTableRow>> StageMonsterGroup;

	TWeakObjectPtr<UStageTowerContext> FindNormalTowerContext(int32 Kind, int32 Level);
	TOptional<int32> FindNormalTowerMaxLevel(int32 Kind);
	TWeakObjectPtr<UStageMonsterContext> FindNormalMonsterContext(int32 Key);
};

UCLASS()
class MY_API UStageTableHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static FGErrorInfo GetBuildStageTower(const UObject* WorldContextObject, EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static FGErrorInfo GetNextStageTower(const UObject* WorldContextObject, EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static FGErrorInfo GetStageTowerMaxLevel(const UObject* WorldContextObject, EStageTowerType TowerType, int32 Kind, int32& Result);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static FGErrorInfo GetStageTowerBaseStats(const UObject* WorldContextObject, EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static FGErrorInfo GetStageMonsterInfo(const UObject* WorldContextObject, int32 MonsterKey, FStageMonsterInfo& Result);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static FGErrorInfo GetStageMonsterBaseStats(const UObject* WorldContextObject, int32 MonsterKey, TMap<EStageUnitAttribute, double>& Result);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static FGErrorInfo GetWaveGroupInfo(const UObject* WorldContextObject, int32 WaveGroup, TArray<FStageWaveGroupInfo>& Result);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static FGErrorInfo GetMonsterGroupInfo(const UObject* WorldContextObject, int32 MonsterGroup, TArray<FMonsterGroupTableRow>& Result);
};
