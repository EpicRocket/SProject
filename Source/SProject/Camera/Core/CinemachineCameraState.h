

#pragma once

#include "CoreMinimal.h"
#include "CinemachineLensSettings.h"
#include "CinemachineCameraState.generated.h"

UENUM(BlueprintType)
enum class ECVStageType : uint8
{
	//First stage: position the camera in space
	Body = 0,
	//Second stage: orient the camera to point at the target
	Aim = 1,
	//Final pipeline stage: apply noise (this is done separately, in the Correction channel of the CameraState)
	Noise = 2,
	// Post-correction stage.  This is invoked on all virtual camera types, after the pipeline is complete
	Finalize = 3,
};


UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class ECVBlendHint
{
	None = 0x00								UMETA(DisplayName = "없음", ToolTip = "일반 상태 블렌딩"),
	SphericalLocation = 0x01				UMETA(DisplayName = "구형 블렌드", ToolTip = "LookAt에 대한 구형 블렌드(있는 경우)"),
	CylindriacalLocation = 0x02				UMETA(DisplayName = "원통현 블렌드", ToolTip = "LookAt에 대한 원통형 블렌드(있는 경우)"),
	ScreenSpaceAimWhenTargetsDiffer = 0x04	UMETA(DisplayName = "방사형 블렌드", ToolTip = "LookAt이 변경될 때 방사형 블렌드(있는 경우)"),
	InheritPosition = 0x10					UMETA(DisplayName = "이전 카메라 위치 상속", ToolTip = "이전에 라이브 중이였던 카메라의 위치를 상속합니다."),
	IgnoreLookAtTarget = 0x20				UMETA(DisplayName = "구형 보간", ToolTip = "블렌딩할 때 추적된 목표를 고려하지 않고 구형 보간을 수행"),
	FreezeWhenBlendingOut = 0x40			UMETA(DisplayName = "블렌딩 시 멈춤", ToolTip = "이 카메라에서 블렌드 아웃할 때는 라이브 상태 대신에 라이브 상태 대신 발신 상태의 스냅샷을 사용"),
};

/**
 * 가상 카메라 블렌딩에 대한 힌트 값 정의
 */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class ECVBlendHintValue
{
	None = 0x00,
	SphericalLocationBlend = 0x01,
	CylindricalLocationBlend = 0x02,
	ScreenSpaceAimWhenTargetsDiffer = 0x04,
	InheritPosition = 0x10,
	IgnoreLookAtTarget = 0x20,
	FreezeWhenBlendingOut = 0x40,

	NoLocation = 1 << 16,
	NoOrientation = 2 << 16,
	NoTransform = NoLocation | NoOrientation,
	NoLens = 4 << 16,
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
		, RotationDampingBypass(FRotator::ZeroRotator)
		, ShotQuality(1.0f)
		, LocationCorrection(FVector::ZeroVector)
		, OrientationCorrection(FRotator::ZeroRotator)
		, BlendHint(0)
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

	FVector GetFinalLocation() const
	{
		return RawLocation + LocationCorrection;
	}

	FRotator GetFinalOrientation() const
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
	FRotator RotationDampingBypass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	float ShotQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FVector LocationCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State")
	FRotator OrientationCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State", meta = (DisplayName = "블렌드 형식", Bitmask, BitmaskEnum = "/Script/Ccg.ECVBlendHintValue"))
	int32 BlendHint;
};

FRotator ApplyCameraRotation(const FQuat& Orient, const FVector2D& Rot, const FVector& WorldUp);

FVector2D GetCameraRotationToTarget(const FQuat& Orient, FVector LookAtDir, FVector WorldUp);
