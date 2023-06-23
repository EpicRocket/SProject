

#pragma once

#include "CoreMinimal.h"
#include "CinemachineLensSettings.h"
#include "CinemachineCameraState.generated.h"

/**
 * 가상 카메라 블렌딩에 대한 힌트 값 정의
*/
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

/**
 * 가상 카메라의 상태를 나타내는 구조체
*/
USTRUCT(BlueprintType)
struct FCinemachineCameraState
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

	FRotator FinalOrientation() const
	{
		if (FMath::Abs(Lens.Dutch) > UE_KINDA_SMALL_NUMBER)
		{
			return (GetCorrectedOrientation().Quaternion() * FQuat(FVector::ForwardVector, FMath::DegreesToRadians(Lens.Dutch))).Rotator();
		}
		return GetCorrectedOrientation();
	}

	FVector InterpolateLocation(FVector LocationA, FVector PivotA, FVector LocationB, FVector PivotB, float Alpha) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FCinemachineLensSettings Lens;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FVector ReferenceUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FVector ReferenceLookAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FVector RawLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FRotator RawOrientation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FVector LocationDampingBypass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	float ShotQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FVector LocationCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FRotator OrientationCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	ECinemachineBlendHintValue BlendHint;
};
