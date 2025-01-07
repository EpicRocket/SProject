
#pragma once

#include "CoreMinimal.h"
#include "Error/GErrorTypes.h"

#include "StageTowerTypes.generated.h"

struct FBuildStageTower;

USTRUCT(BlueprintType)
struct FStageTowerReceipt
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintType)
    TArray<FBuildStageTower> BuildTowers;

    UPROPERTY(EditAnywhere, BlueprintType)
	FGErrorInfo Error;

    UPROPERTY(EditAnywhere, BlueprintType)
    bool bMaxLevel = false;

    UPROPERTY(EditAnywhere, BlueprintType)
    bool bSellable = false;

    UPROPERTY(EditAnywhere, BlueprintType)
    int64 SellPrice = 0;
};
