

#pragma once

#include "Camera/CinemachineBaseStage.h"
#include "CinemachineComposer.generated.h"

USTRUCT(BlueprintType)
struct FCVComposerData
{
	GENERATED_BODY()

public:
	FCVComposerData()
		: TrackedObjectOffset(FVector::ZeroVector)
		, LookaheadTime(0.0F)
		, LookaheadSmoothing(0.0F)
		, LookaheadIgnoreY(false)
		, HorizontalDamping(0.5F)
		, VerticalDamping(0.5F)
		, ScreenX(0.5F)
		, ScreenY(0.5F)
		, DeadZoneWidth(0.0F)
		, DeadZoneHeight(0.0F)
		, SoftZoneWidth(0.8F)
		, SoftZoneHeight(0.8F)
		, BiasX(0.0F)
		, BiasY(0.0F)
		, bCenterOnActivate(true)
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TrackedObjectOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LookaheadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "30.0"))
	float LookaheadSmoothing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LookaheadIgnoreY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float HorizontalDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float VerticalDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-0.5", ClampMax = "1.5"))
	float ScreenX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-0.5", ClampMax = "1.5"))
	float ScreenY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float DeadZoneWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float DeadZoneHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float SoftZoneWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float SoftZoneHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-0.5", ClampMax = "0.5"))
	float BiasX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-0.5", ClampMax = "0.5"))
	float BiasY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCenterOnActivate;
};

/**
 * 가상 카메라의 LookAt 타겟에 대한 화면 회전을 담당하는 컴포넌트
 */
UCLASS(BlueprintType, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineComposer : public UCinemachineBaseStage
{
	GENERATED_BODY()

public:
	UCinemachineComposer();

	//~ Begin UCinemachineBaseStage interface
	virtual void PrePipelineMutateCameraState(FCinemachineCameraState& CurState, float DeltaTime) override;

	virtual ECVStage GetStage() const override;

	virtual void MutateCameraState(FCinemachineCameraState& State, float DeltaTime) override;

	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;

	virtual float GetMaxDampTime() const override;
	//~ End UCinemachineBaseStage interface

	virtual FVector GetLookAtPointAndSetTrackedPoint(FVector LookAt, FVector Up, float DeltaTime);

	FBox2D GetSoftGuidRect();

	void SetSoftGuidRect(FBox2D Rect);

	FBox2D GetHardGuidRect();

	void SetHardGuidRect(FBox2D Rect);

	void RotateToScreenBounds(FCinemachineCameraState& State, FBox2D ScreenRect, FVector TargetPoint, FRotator& RigOrientation, float FOV, float FOVH, float DeltaTime);

	void ClampVerticalBounds(FBox2D& Rect, FVector Direction, FVector Up, float FOV);

protected:
	//~ Begin UCinemachineBaseStage interface
	virtual void OnInitialize() override;
	//~ End UCinemachineBaseStage interface

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Stage")
	FCVComposerData ComposerData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Stage")
	FVector TrackedPoint;

private:
	FVector CameraLocationPrevFrame = FVector::ZeroVector;
	FVector LookAtPrevFrame = FVector::ZeroVector;
	FVector2D ScreenOffsetPreFrame = FVector2D::ZeroVector;
	FRotator CameraOrientationPrevFrame = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, Category = "Cinemachine|Component|Composer", meta = (AllowPrivateAccess = "true"))
	class ULocationPredictor* Predictor;

	TSharedPtr<struct FFOVCache> FOVCache;
};
