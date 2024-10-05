
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/EngineSubsystem.h"
#include "Templates/SharedPointer.h"

#include "StageTableRepository.generated.h"

struct FTowerTableRow;
struct FBuildStageTower;

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
	TMap<int32/*Kind*/, TMap<int32/*Level*/, TSharedPtr<FTowerTableRow>>> TowerTableRows;
};

UCLASS()
class MY_API UStageTableHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "스테이지|타워|건설", meta = (ReturnDisplayName = "Find"))
    bool GetBuildStageTower(int32 Kind, int32 Level, FBuildStageTower& Result);
};
