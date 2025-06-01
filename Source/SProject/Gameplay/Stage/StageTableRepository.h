
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Table/Subsystem/GTableRepositorySubsystem.h"
#include "Templates/SharedPointer.h"

#include "StageTableRepository.generated.h"

enum class EStageTowerType : uint8;
enum class EStageUnitAttribute : uint8;
struct FGErrorInfo;
struct FNormalTowerTableRow;
struct FStageTowerInfo;
struct FStageTableRow;
struct FStageMonsterInfo;
struct FStageWaveGroupInfo;
struct FStageMonsterGroupInfo;
struct FMonsterGroupTableRow;
class AStageTowerUnit;
class UWorld;

UCLASS()
class MY_API UStageTableRepository : public UGTableRepositorySubsystem
{
	GENERATED_BODY()

	friend UStageTableHelper;
	static UStageTableRepository* Get();
public:
	virtual void Load() override;
	virtual void Unload() override;

private:
	TMap<int32/*Kind*/, TSortedMap<int32/*Level*/, TSharedPtr<FNormalTowerTableRow>>> NormalTowerTableRows;
	TSortedMap<int32/*Level*/, TSharedPtr<FNormalTowerTableRow>>* FindNormalTowerTableRows(int32 Kind);
	TSharedPtr<FNormalTowerTableRow>* FindNormalTowerTableRow(int32 Kind, int32 Level);
	
	TMap<int32/*Kind*/, TSortedMap<int32/*Level*/, TSharedPtr<FStageTowerInfo>>> NormalTowerInfos;
	TSortedMap<int32/*Level*/, TSharedPtr<FStageTowerInfo>>* FindNormalTowerTableInfos(int32 Kind);
	TSharedPtr<FStageTowerInfo>* FindNormalTowerInfo(int32 Kind, int32 Level);
	
	TSortedMap<int32, TSharedPtr<FStageMonsterInfo>> MonsterInfos;
	TSharedPtr<FStageMonsterInfo> FindMonsterInfo(int32 MonsterKey);

	TMap<int32/*Group*/, TArray<TSharedPtr<FMonsterGroupTableRow>>> MonsterGroupInfos;
	TArray<TSharedPtr<FMonsterGroupTableRow>>* FindMonsterGroupInfo(int32 Group);

	TMap<int32/*Wave_Group*/, TArray<TSharedPtr<FStageWaveGroupInfo>>> WaveGroupInfos;
	TArray<TSharedPtr<FStageWaveGroupInfo>>* FindWaveGroupInfo(int32 WaveGroup);
};

UCLASS()
class MY_API UStageTableHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure, meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure, meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetStageTowerSellPrice(EStageTowerType TowerType, int32 Kind, int32 Level, int64& Result);

	UFUNCTION(BlueprintPure)
	static int32 GetStageTowerMaxLevel(EStageTowerType TowerType, int32 Kind);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetStageTowerBaseStats(EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetStageMonsterInfo(int32 MonsterKey, FStageMonsterInfo& Result);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetStageMonsterBaseStats(int32 MonsterKey, TMap<EStageUnitAttribute, double>& Result);

	UFUNCTION(BlueprintPure, meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetStage(int32 Level, FStageTableRow& Result);

	UFUNCTION(BlueprintPure, meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetStageMap(int32 Level, TSoftObjectPtr<UWorld>& Map);

	UFUNCTION(BlueprintPure, meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetWaveGroupInfo(int32 WaveGroup, TArray<FStageWaveGroupInfo>& Result);

	UFUNCTION(BlueprintPure, meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetMonsterGroupInfo(int32 MonsterGroup, TArray<FMonsterGroupTableRow>& Result);
};
