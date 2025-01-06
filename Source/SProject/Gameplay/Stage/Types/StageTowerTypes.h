
#pragma once

#include "CoreMinimal.h"

#include "StageTowerTypes.generated.h"

struct FBuildStageTower;

USTRUCT(BlueprintType)
struct FStageTowerReceipt
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintType)
    TArray<FBuildStageTower> BuildTowers;

    UPROPERTY(EditAnywhere, BlueprintType)
    int64 SalesPrice = 0;
};
