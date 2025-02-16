
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/EngineSubsystem.h"
#include "Templates/SharedPointer.h"

#include "StageTableRepository.generated.h"

enum class EStageTowerType : uint8;
enum class EStageUnitAttribute : uint8;
struct FNormalTowerTableRow;
struct FStageTowerInfo;
struct FStageTableRow;
class AStageTowerUnit;
class UWorld;

UCLASS()
class MY_API UStageTableRepository : public UEngineSubsystem
{
	GENERATED_BODY()

	friend UStageTableHelper;
public:
	static UStageTableRepository* Get();

	UFUNCTION(BlueprintCallable)
	void Load();

	UFUNCTION(BlueprintCallable)
	void Unload();

private:
	TMap<int32/*Kind*/, TSortedMap<int32/*Level*/, TSharedPtr<FNormalTowerTableRow>>> NormalTowerTableRows;
	TSortedMap<int32/*Level*/, TSharedPtr<FNormalTowerTableRow>>* FindNormalTowerTableRows(int32 Kind);
	TSharedPtr<FNormalTowerTableRow>* FindNormalTowerTableRow(int32 Kind, int32 Level);

	TSortedMap<int32/*Level*/, TSharedPtr<FStageTableRow>> StageTableRows;
	TSharedPtr<FStageTableRow> FindStageTableRow(int32 Level);
};

UCLASS()
class MY_API UStageTableHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "스테이지|타워|건설", meta = (ReturnDisplayName = "Find"))
	static bool GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워|건설", meta = (ReturnDisplayName = "Find"))
	static bool GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워", meta = (ReturnDisplayName = "Find"))
	static bool GetStageTowerUnitClass(EStageTowerType TowerType, int32 Kind, int32 Level, TSubclassOf<AStageTowerUnit>& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워", meta = (ReturnDisplayName = "Find"))
	static bool GetStageTowerSellPrice(EStageTowerType TowerType, int32 Kind, int32 Level, int64& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워")
	static int32 GetStageTowerMaxLevel(EStageTowerType TowerType, int32 Kind);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워")
	static bool GetStageTowerBaseStats(EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|정보", meta = (ReturnDisplayName = "Find"))
	static bool GetStage(int32 Level, FStageTableRow& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|정보", meta = (ReturnDisplayName = "Find"))
	static bool GetStageMap(int32 Level, TSoftObjectPtr<UWorld>& Map);
};
