

#pragma once

#include "Camera/CinemachineBaseStage.h"
#include "CinemachineTransposer.generated.h"

UENUM(BlueprintType)
enum class ECVBindingMode : uint8
{
	LockToTargetOnAssign		UMETA(ToolTip = "카메라가 {FollowTarget}에 바인딩. 가상 카메라가 활성화된 시점의 대상 로컬 프레임으로 구성 또는 {FollowTarget}이 할당된 시점의 로컬 프레임으로 구성된 참조 프레임을 사용함"),
	LockToTargetWithWorldUp		UMETA(ToolTip = "카메라가 {FollowTarget}의 로컬 프레임으로 구성된 기준 프레임을 사용하여 {FollowTarget}에 바인딩, {Target}의 로컬 프레임으로 구성된 기준 프레임을 사용하여 {FollowTarget}을 추적"),
	LockToTargetNoRoll			UMETA(ToolTip = "카메라는 다음과 같이 구성된 기준 프레임을 사용하여 {FollowTarget}에 바인딩. {FollowTarget}의 로컬 프레임으로 구성된 기준 프레임을 사용하여 {FollowTarget}을 추적"),
	LockToTarget				UMETA(ToolTip = "카메라는 {FollowTarget}의 로컬 프레임을 사용하여 {FollowTarget}에 바인딩"),
	WorldSpace					UMETA(ToolTip = "카메라는 월드 스페이스 오프셋을 사용하여 {FollowTarget}에 바인딩"),
	SimpleFollowWithWorldUp		UMETA(ToolTip = "오프셋은 카메라-로컬 축을 사용하여 {FollowTarget}을 기준으로 추적"),
};

USTRUCT(BlueprintType)
struct FCVTransposerData
{
	GENERATED_BODY()

public:
	FCVTransposerData()
		: BindingMode(ECVBindingMode::LockToTargetWithWorldUp)
		, FollowOffset(FVector::BackwardVector * 300.0f)
		, XDamping(1.0F)
		, YDamping(1.0F)
		, ZDamping(1.0F)
		, PitchDamping(0.0F)
		, YawDamping(0.0F)
		, RollDamping(0.0F)
		, AngularDamping(0.0F)
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECVBindingMode BindingMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FollowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 20.0))
	float XDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 20.0))
	float YDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 20.0))
	float ZDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 20.0))
	float PitchDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 20.0))
	float YawDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 20.0))
	float RollDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 20.0))
	float AngularDamping;
};

/**
 * 가상 카메라의 Follow 타겟에 대한 상대적인 위치를 조정하는 컴포넌트
 */
UCLASS(BlueprintType, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineTransposer : public UCinemachineBaseStage
{
	GENERATED_BODY()

public:
	//~ Begin UCinemachineBaseStage interface
	virtual ECVStage GetStage() const override;
	virtual void MutateCameraState(FCinemachineCameraState& State, float DeltaTime) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual float GetMaxDampTime() const override;
	//~ End UCinemachineBaseStage interface

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Stage", meta = (ExposeOnSpawn))
	FCVTransposerData TransposerData;

private:
	FVector PreviousTargetLocation = FVector::ZeroVector;

	FRotator PreviousReferenceOrientation = FRotator::ZeroRotator;

	FRotator TargetOrientationOnAssign = FRotator::ZeroRotator;

	FVector PreviousOffset = FVector::ZeroVector;

	UPROPERTY(Transient)
	USceneComponent* PreviousTarget = nullptr;
};
