

#pragma once

#include "CoreMinimal.h"
#include "CinemachineLookaheadSettings.generated.h"

USTRUCT(BlueprintType)
struct FCinemachineLookaheadSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "활성화"))
    bool bEnabled = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bEnabled, ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0, Units = "s"))
    float Time = 0.0F;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bEnabled, ClampMin = 0.0, ClampMax = 30.0, UIMin = 0.0, UIMax = 30.0, Tooltip = "룩헤드 알고리즘의 부드러움을 제어합니다. 값이 클수록 불안정한 예측이 부드러워지고 예측 지연도 증가합니다."))
    float Smoothing = 0.0F;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bEnabled, Tooltip = "이 옵션을 선택하면 룩헤드 계산 시 Y축을 따라 이동이 무시됩니다."))
    bool IgnoreY = false;
};
