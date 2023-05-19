
#pragma once

#include "CoreMinimal.h"
#include "CinemachineCameraState.generated.h"

USTRUCT(BlueprintType)
struct SPROJECT_API FCinemachineCameraState
{
    GENERATED_USTRUCT_BODY()

public:

    FVector ReferenceUp;

    FVector ReferenceLookAt;

    bool HasLookAt;

    FVector RawLocation;

    FRotator RawOrientation;

    FVector LocationDampingBypass;

    float ShotQuality;

    FVector LocationCorrection;

    FRotator OrientationCorrection;

    FVector GetCorrectedPosition() const
    {
        return RawLocation + LocationCorrection;
    }

    FRotator GetCorrectedOrientation() const
    {
        return RawOrientation + OrientationCorrection;
    }

    FVector FinalLocation() const;

    
};
