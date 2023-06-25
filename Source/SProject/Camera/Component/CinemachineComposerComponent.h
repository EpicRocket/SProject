

#pragma once

#include "Camera/CinemachineBaseComponent.h"
#include "CinemachineComposerComponent.generated.h"

/**
 * 가상 카메라의 LookAt 타겟에 대한 화면 회전을 담당하는 컴포넌트
 */
UCLASS(Blueprintable, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineComposerComponent : public UCinemachineBaseComponent
{
	GENERATED_BODY()

public:
	UCinemachineComposerComponent();

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	//~ End UActorComponent interface

	//~ Begin UCinemachineBaseComponent interface
	virtual void PrePipelineMutateCameraState(FCinemachineCameraState& CurState, float DeltaTime) override;

	virtual ECinemachineStage GetStage() const override;

	virtual void MutateCameraState(FCinemachineCameraState& State, float DeltaTime) override;

	virtual void OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta) override;

	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;

	virtual float GetMaxDampTime() const override;
	//~ End UCinemachineBaseComponent interface

	virtual FVector GetLookAtPointAndSetTrackedPoint(FVector LookAt, FVector Up, float DeltaTime);

	FBox2D GetSoftGuidRect();

	void SetSoftGuidRect(FBox2D Rect);

	FBox2D GetHardGuidRect();

	void SetHardGuidRect(FBox2D Rect);

	void RotateToScreenBounds(FCinemachineCameraState& State, FBox2D ScreenRect, FVector TargetPoint, FRotator& RigOrientation, float FOV, float FOVH, float DeltaTime);

	void ClampVerticalBounds(FBox2D& Rect, FVector Direction, FVector Up, float FOV);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer")
	FVector TrackedObjectOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LookaheadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "0.0", ClampMax = "30.0"))
	float LookaheadSmoothing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer")
	bool LookaheadIgnoreY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float HorizontalDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float VerticalDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "-0.5", ClampMax = "1.5"))
	float ScreenX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "-0.5", ClampMax = "1.5"))
	float ScreenY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float DeadZoneWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float DeadZoneHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float SoftZoneWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float SoftZoneHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "-0.5", ClampMax = "0.5"))
	float BiasX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer", meta = (ClampMin = "-0.5", ClampMax = "0.5"))
	float BiasY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer")
	bool bCenterOnActivate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Composer")
	FVector TrackedPoint;

private:
	FVector CameraLocationPrevFrame;
	FVector LookAtPrevFrame;
	FVector2D ScreenOffsetPreFrame;
	FRotator CameraOrientationPrevFrame;

	UPROPERTY(VisibleAnywhere, Category = "Cinemachine|Component|Composer", meta = (AllowPrivateAccess = "true"))
	class ULocationPredictor* Predictor;

	TSharedPtr<struct FFOVCache> FOVCache;
};
