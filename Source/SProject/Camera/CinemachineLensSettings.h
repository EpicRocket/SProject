
#pragma once

#include "CoreMinimal.h"
#include "CinemachineLensSettings.generated.h"


USTRUCT(BlueprintType)
struct SPROJECT_API FCinemachineLensSettings
{
    GENERATED_USTRUCT_BODY()

public:
    FCinemachineLensSettings(float InFieldOfView, float InOrthoWidth, float InOrthoNearClipPlane, float InOrthoFarClipPlane, float InAspectRatio, float InDutch)
        : FiledOfView(InFieldOfView)
        , AspectRatio(InAspectRatio)
        , Dutch(InDutch)
    {
    }

    FCinemachineLensSettings()
        : FCinemachineLensSettings(90.0f, 1.7777778f, 0.0f)
    {
    }

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LensSettgins, meta = (UIMin = "5.0", UIMax = "170", ClampMin = "0.001", ClampMax = "360.0", Units = deg))
    float FiledOfView;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LensSettgins, meta = (ClampMin = "0.1", ClampMax = "100.0", EditCondition = "bConstrainAspectRatio"))
    float AspectRatio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LensSettgins, meta = (ClampMin = "-180.0", ClampMax = "180.0"))
    float Dutch;
};
