
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Table/TableRepositorySubsystem.h"
#include "Templates/SharedPointer.h"

#include "StageTableRepository.generated.h"

enum class EStageTowerType : uint8;
enum class EStageUnitAttribute : uint8;
struct FNormalTowerTableRow;
struct FStageTowerInfo;
struct FStageTableRow;
struct FGErrorInfo;
class AStageTowerUnit;
class UWorld;

UCLASS()
class MY_API UStageTableRepository : public UTableRepositorySubsystem
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

	TSortedMap<int32/*Level*/, TSharedPtr<FStageTableRow>> StageTableRows;
	TSharedPtr<FStageTableRow> FindStageTableRow(int32 Level);
};

UCLASS()
class MY_API UStageTableHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "스테이지|타워|건설", meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워|건설", meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FStageTowerInfo& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워", meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetStageTowerUnitClass(EStageTowerType TowerType, int32 Kind, int32 Level, TSubclassOf<AStageTowerUnit>& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워", meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetStageTowerSellPrice(EStageTowerType TowerType, int32 Kind, int32 Level, int64& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워")
	static int32 GetStageTowerMaxLevel(EStageTowerType TowerType, int32 Kind);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워")
	static FGErrorInfo GetStageTowerBaseStats(EStageTowerType TowerType, int32 Kind, int32 Level, TMap<EStageUnitAttribute, double>& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|정보", meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetStage(int32 Level, FStageTableRow& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|정보", meta = (ReturnDisplayName = "Find"))
	static FGErrorInfo GetStageMap(int32 Level, TSoftObjectPtr<UWorld>& Map);
};
