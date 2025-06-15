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

struct FStageMonsterGroupInfo;

struct FStageWaveGroupInfo;

UCLASS()
class UStageTableReceipt
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UStageTowerContext*> NormalTowerContexts;

	UPROPERTY()
	TArray<UStageMonsterContext*> MonsterContexts;
};

UCLASS()
class MY_API UStageTableRepository : public UGTableRepositorySubsystem
{
	GENERATED_BODY()

	friend UStageTableHelper;
public:
	static UStageTableRepository* Get(const UObject* WorldContextObject);

	TFuture<UStageTableReceipt*> Load(int32 StageLevel, TMap<EStageTowerType, TSet<int32>> TowerList);

	void Unload(const UStageTableReceipt* Receipt);

private:
	TWeakObjectPtr<UStageTowerContext> FindNormalTowerContext(int32 Kind, int32 Level) const;
	TOptional<int32> FindNormalTowerMaxLevel(int32 Kind) const;
	TWeakObjectPtr<UStageMonsterContext> FindNormalMonsterContext(int32 Key) const;

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
