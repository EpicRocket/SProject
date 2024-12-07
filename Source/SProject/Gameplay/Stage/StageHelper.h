
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "StageHelper.generated.h"

struct FStageTableRow;

UCLASS()
class MY_API UStageHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "스테이지|Helper", meta = (WorldContext = "WorldContextObject"))
    static bool GetCurrentStageLevel(const UObject* WorldContextObject, int32& Level);

    UFUNCTION(BlueprintPure, Category = "스테이지|Helper", meta = (WorldContext = "WorldContextObject"))
    static bool GetCurrentStageUsePoint(const UObject* WorldContextObject, int32& UsePoint);
};
