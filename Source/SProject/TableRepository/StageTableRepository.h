
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/EngineSubsystem.h"
#include "Templates/SharedPointer.h"

#include "StageTableRepository.generated.h"

struct FNormalTowerTableRow;
struct FBuildStageTower;
struct FStageTableRow;
enum class EStageTowerType : uint8;

UCLASS()
class MY_API UStageTableRepository : public UEngineSubsystem
{
    GENERATED_BODY()

    friend UStageTableHelper;
public:
	static UStageTableRepository* Get();

    void Load();

    void Unload();

private:
	TMap<int32/*Kind*/, TSortedMap<int32/*Level*/, TSharedPtr<FNormalTowerTableRow>>> NormalTowerTableRows;
    TSortedMap<int32/*Level*/, TSharedPtr<FNormalTowerTableRow>>* FindNormalTowerTableRows(int32 Kind);
	TSharedPtr<FNormalTowerTableRow>* FindNormalTowerTableRow(int32 Kind, int32 Level);

    TSortedMap<int32/*Level*/, TSharedPtr<FStageTableRow>> StageTableRows;
};

UCLASS()
class MY_API UStageTableHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "스테이지|타워|건설", meta = (ReturnDisplayName = "Find"))
    static bool GetBuildStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FBuildStageTower& Result);

    UFUNCTION(BlueprintPure, Category = "스테이지|타워|건설", meta = (ReturnDisplayName = "Find"))
    static bool GetNextStageTower(EStageTowerType TowerType, int32 Kind, int32 Level, FBuildStageTower& Result);

	UFUNCTION(BlueprintPure, Category = "스테이지|타워|판매", meta = (ReturnDisplayName = "Find"))
	static bool GetStageTowerSellPrice(EStageTowerType TowerType, int32 Kind, int32 Level, int64& Result);

    UFUNCTION(BlueprintPure, Category = "스테이지|타워")
    static int32 GetStageTowerMaxLevel(EStageTowerType TowerType, int32 Kind);

    UFUNCTION(BlueprintPure, Category = "스테이지|정보", meta = (ReturnDisplayName = "Find"))
    static bool GetStageTableInfo(int32 Level, FStageTableRow& Result);
};