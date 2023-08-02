

#pragma once

#include "CoreMinimal.h"
#include "CinemachineTargetTracking.generated.h"

UENUM(BlueprintType)
enum class ECVBindingMode : uint8
{
	LockToTargetOnAssign = 0    UMETA(Tooltip = "카메라는 가상 카메라가 활성화된 시점의 대상의 로컬 프레임으로 구성된 참조 프레임을 사용하여 팔로우 대상에 바인딩됩니다 또는 타겟이 할당된 시점의 로컬 프레임으로 구성된 참조 프레임을 사용합니다."),
	LockToTargetWithWorldUp = 1 UMETA(Tooltip = "카메라는 다음과 같이 구성된 기준 프레임을 사용하여 팔로우 타겟에 바인딩됩니다. 로 구성된 기준 프레임을 사용하여 타깃을 따라갑니다."),
	LockToTargetNoRoll = 2      UMETA(Tooltip = "카메라는 대상의 로컬 프레임으로 구성된 기준 프레임을 사용하여 팔로우 대상에 바인딩되며 롤은 제로가 됩니다."),
	LockToTarget = 3            UMETA(Tooltip = "카메라는 대상의 로컬 프레임을 사용하여 팔로우 대상에 바인딩됩니다."),
	WorldSpace = 4              UMETA(Tooltip = "카메라는 월드 스페이스 오프셋을 사용하여 팔로우 타깃에 바인딩됩니다."),
	LazyFollow = 5              UMETA(Tooltip = "오프셋은 카메라-로컬 축을 사용하여 타깃을 기준으로 계산됩니다.")
};

UENUM(BlueprintType)
enum class ECVAngularDampingMode
{
	// Use Euler angles to specify damping values. Subject to gimbal-lock when pitch is steep.
	Euler,
	// Use quaternions to calculate angular damping. No per-channel control, but not susceptible to gimbal-lock
	Quaternion
};

class USceneComponent;
class UCinemachineBaseStage;

USTRUCT(BlueprintType)
struct FCinemachineTrackerSettings
{
	GENERATED_BODY()

public:
	FCinemachineTrackerSettings() = default;

	void Validate()
	{
		for(int32 i = 0; i < 3; ++i)
		{
			LocationDamping[i] = FMath::Max(0.0F, LocationDamping[i]);
			RotationDamping[i] = FMath::Max(0.0F, RotationDamping[i]);
		}
		QuaternionDamping = FMath::Max(0.0F, QuaternionDamping);
	}
	
	float GetMaxDampTime() const
	{
		const FVector DampLocation = GetEffectiveLocationDamping();
		const FVector DampRotation = AngularDampingMode == ECVAngularDampingMode::Euler ? GetEffectiveRotationDamping() : FVector(0.0F, QuaternionDamping, 0.0F);
		return FMath::Max(FMath::Max3(DampLocation.X, DampLocation.Y, DampLocation.Z), FMath::Max3(DampRotation.X, DampRotation.Y, DampRotation.Z));
	}

	FVector GetEffectiveLocationDamping() const
	{
		return BindingMode == ECVBindingMode::LazyFollow ? FVector(LocationDamping.X, 0.0F, LocationDamping.Z) : LocationDamping;
	}

	FVector GetEffectiveRotationDamping() const
	{
		switch (BindingMode)
		{
		case ECVBindingMode::LockToTargetNoRoll: return FVector(0.0F, RotationDamping.Y, RotationDamping.Z);

		case ECVBindingMode::LockToTargetWithWorldUp: return FVector(0.0F, 0.0F, RotationDamping.Z);

		case ECVBindingMode::WorldSpace:
		case ECVBindingMode::LazyFollow:
			return FVector::ZeroVector;

		default: return RotationDamping;
		}
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECVBindingMode BindingMode = ECVBindingMode::WorldSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "축별로 카메라가 오프셋을 얼마나 적극적으로 유지하려고 하는가입니다. 숫자가 작을수록 반응이 빨라져 카메라가 타겟의 오프셋을 유지하기 위해 빠르게 전환합니다. 숫자가 클수록 카메라가 느리게 반응합니다. 축별로 다른 설정을 사용하면 다양한 카메라 동작을 구현할 수 있습니다."))
	FVector LocationDamping = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECVAngularDampingMode AngularDampingMode = ECVAngularDampingMode::Euler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "카메라가 대상의 회전을 얼마나 적극적으로 추적하려고 시도하는지, 축별로 나타냅니다. 숫자가 작을수록 반응이 빠릅니다. 숫자가 클수록 느리게 반응하는 카메라입니다."))
	FVector RotationDamping = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 20.0, UIMin = 0.0, UIMax = 20.0, Tooltip = "카메라가 대상의 회전을 얼마나 적극적으로 추적하려고 하는지를 나타냅니다. 숫자가 작을수록 반응이 빠릅니다. 숫자가 클수록 느리게 반응하는 카메라입니다."))
	float QuaternionDamping = 0.0F;
	
};

USTRUCT(BlueprintType)
struct FCinemachineTracker
{
	GENERATED_BODY()

public:
	FCinemachineTracker() = default;

    void InitState(UCinemachineBaseStage* Stage, float DeltaTime, ECVBindingMode BindingMode, FVector WorldUp);

	FRotator GetReferenceOrientation(UCinemachineBaseStage* Stage, ECVBindingMode BindingMode, FVector WorldUp) const;

    void TrackTarget(UCinemachineBaseStage* Stage, float DeltaTime, FVector WorldUp, FVector DesiredCameraOffset, FCinemachineTrackerSettings& InSettings, FVector& OutTargetLocation, FRotator& OutTargetOrientation);

    FVector GetOffsetForMinimumDistance(UCinemachineBaseStage* Stage, FVector DampedTargetLocation, FVector CameraOffset, FVector CameraFoward, FVector WorldUp, FVector ActualTargetLocation);

    void ForceCameraLocation(UCinemachineBaseStage* Stage, ECVBindingMode BindingMode, FVector Location, FRotator Orientation, FVector CameraOffsetLocalSpace);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PreviousTargetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator PreviousReferenceOrientation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TargetOrientationOnAssign = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PreviousOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USceneComponent* PreviousTarget = nullptr;
};
