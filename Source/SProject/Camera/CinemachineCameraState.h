
#pragma once

#include "CoreMinimal.h"
#include "CinemachineLensSettings.h"
#include "CinemachineCameraState.generated.h"

UENUM(BlueprintType)
enum class ECinemachineBlendHintValue : uint8
{
	Nothing = 0x00,
	NoLocation = 0x01,
	NoOrientation = 0x02,
	NoTransform = NoLocation | NoOrientation,
	SphericalLocationBlend = 0x04,
	CylindricalLocationBlend = 0x08,
	RadialAimBlend = 0x10,
	IgnoreLookAtTarget = 0x20,
	NoLens = 0x40,
};

USTRUCT(BlueprintType)
struct SPROJECT_API FCinemachineCameraState
{
	GENERATED_USTRUCT_BODY()

public:
	static FCinemachineCameraState Lerp(const FCinemachineCameraState& A, const FCinemachineCameraState& B, float Alpha);

public:
	FCinemachineCameraState()
		: ReferenceUp(FVector::UpVector)
		, ReferenceLookAt(FVector::ZeroVector)
		, RawLocation(FVector::ZeroVector)
		, RawOrientation(FRotator::ZeroRotator)
		, LocationDampingBypass(FVector::ZeroVector)
		, ShotQuality(1.0f)
		, LocationCorrection(FVector::ZeroVector)
		, OrientationCorrection(FRotator::ZeroRotator)
		, BlendHint(ECinemachineBlendHintValue::Nothing)
	{
	}

    bool HasLookAt() const
    {
        return ReferenceLookAt != FVector::ZeroVector;
    }

	FVector GetCorrectedLocation() const
	{
		return RawLocation + LocationCorrection;
	}

	FRotator GetCorrectedOrientation() const
	{
		return RawOrientation + OrientationCorrection;
	}

	FVector FinalLocation() const
	{
		return RawLocation + LocationCorrection;
	}

	FVector InterpolateLocation(FVector LocationA, FVector PivotA, FVector LocationB, FVector PivotB, float Alpha) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	FCinemachineLensSettings Lens;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	FVector ReferenceUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	FVector ReferenceLookAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	FVector RawLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	FRotator RawOrientation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	FVector LocationDampingBypass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	float ShotQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	FVector LocationCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	FRotator OrientationCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineCameraState)
	ECinemachineBlendHintValue BlendHint;
};
