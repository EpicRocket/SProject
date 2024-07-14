
#pragma once

#include "CoreMinimal.h"
#include "UnitTypes.h"

#include "StageTypes.generated.h"

struct FStageDetail;

/** 스테이지 정보 */
USTRUCT(BlueprintType)
struct SPROJECT_API FStage
{
    GENERATED_BODY()

    /**스테이지 레벨 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;
};

/** 스테이지 별 상세 정보 */
USTRUCT(BlueprintType)
struct SPROJECT_API FStageDetail
{
    GENERATED_BODY()
    
    /** 스테이지의 레벨(맵) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;

    /** 스테이지에 배치된 타워들 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FTower> Towers;
};

