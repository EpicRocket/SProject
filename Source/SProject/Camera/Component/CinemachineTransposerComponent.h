

#pragma once

#include "CoreMinimal.h"
#include "Camera/CinemachineBaseComponent.h"
#include "CinemachineTransposerComponent.generated.h"

UENUM(BlueprintType)
enum class ECineamchineTransposerBindingMode : uint8
{
	LockToTargetOnAssign,
	LockToTargetWithWorldUp,
	LockToTargetNoRoll,
	LockToTarget,
	WorldSpace,
	SimpleFollowWithWorldUp
};

/**
 * 가상 카메라의 Follow 타겟에 대한 상대적인 위치를 조정하는 컴포넌트
 */

UCLASS(Blueprintable, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineTransposerComponent : public UCinemachineBaseComponent
{
	GENERATED_BODY()

public:
	UCinemachineTransposerComponent();

	//~ Begin UCinemachineBaseComponent interface
	virtual ECinemachineStage GetStage() const override;
	virtual void MutateCameraState(FCinemachineCameraState& State, float DeltaTime) override;
	virtual void OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual float GetMaxDampTime() const override;
	//~ End UCinemachineBaseComponent interface

	FVector GetEffectiveOffset() const;

	FRotator GetReferenceOrientation(FVector WorldUp);

	virtual FVector GetTargetCameraLocation(FVector WorldUp);

protected:
	void InitPrevFrameStateInfo(FCinemachineCameraState& State, float DeltaTime);

	void TrackTarget(float DeltaTime, FVector Up, FVector DesiredCameraOffset, FVector& OutTargetLocation, FRotator& OutTargetOrient);

	FVector GetOffsetForMinimumTargetDistance(FVector DampedTargetLocation, FVector CameraOffset, FVector CameraForward, FVector Up, FVector ActualTargetLocation);

	FVector GetDamping() const;

	FVector GetAngularDamping() const;

public:
	ECineamchineTransposerBindingMode BindingMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer")
	FVector FollowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer", meta = (ClampMin = 0.0, ClampMax = 20.0))
	float XDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer", meta = (ClampMin = 0.0, ClampMax = 20.0))
	float YDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer", meta = (ClampMin = 0.0, ClampMax = 20.0))
	float ZDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer", meta = (ClampMin = 0.0, ClampMax = 20.0))
	float PitchDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer", meta = (ClampMin = 0.0, ClampMax = 20.0))
	float YawDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer", meta = (ClampMin = 0.0, ClampMax = 20.0))
	float RollDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer", meta = (ClampMin = 0.0, ClampMax = 20.0))
	float AngularDamping;

private:
	FVector PreviousTargetLocation = FVector::ZeroVector;

	FRotator PreviousReferenceOrientation = FRotator::ZeroRotator;

	FRotator TargetOrientationOnAssign = FRotator::ZeroRotator;

	FVector PreviousOffset = FVector::ZeroVector;

	UPROPERTY(Transient)
	USceneComponent* PreviousTarget = nullptr;
};
