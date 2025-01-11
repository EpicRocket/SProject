
#pragma once

#include "CoreMinimal.h"
#include "Error/GErrorTypes.h"

#include "StageTowerTypes.generated.h"

struct FBuildStageTower;

USTRUCT(BlueprintType)
struct FStageTowerReceipt
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FBuildStageTower> BuildTowers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGErrorInfo Error;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bMaxLevel = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSellable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 SellPrice = 0;
};
