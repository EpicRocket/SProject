

#pragma once

#include "Camera/Core/CinemachineBaseStage.h"
#include "Camera/Core/CinemachineScreenComposerSettings.h"
#include "Camera/Core/CinemachineLookaheadSettings.h"
#include "Camera/Interface/CinemachineModifierInterface.h"
#include "Camera/Core/CinemachineLensSettings.h"
#include "Shared/LocationPredictor.h"
#include "CinemachineRotationComposer.generated.h"

USTRUCT()
struct FCVFOVCache
{
	GENERATED_BODY()

public:
	static FRect ScreenToFOV(FRect ScreenRect, float FOV, float FOVH, float Aspect);

public:
	void UpdateCache(FCinemachineLensSettings Lens, FRect SoftGuide, FRect HardGuide);

public:
	UPROPERTY()
	FRect FOVSoftGuideRect;

	UPROPERTY()
	FRect FOVHardGuideRect;

	float FOVH = UE_KINDA_SMALL_NUMBER;

	float FOV = UE_KINDA_SMALL_NUMBER;

	float Aspect = 1.0F;

	UPROPERTY()
	FRect DeadZoneRect;

	UPROPERTY()
	FRect HardLimitRect;
};

UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories, ClassGroup = (Cinemachine))
class UCinemachineRotationComposer : public UCinemachineBaseStage, public ICinemachineModifiableCompositionInterface
{
	GENERATED_BODY()

public:
	//~ Begin UCinemachineBaseStage
	virtual bool IsStageValid() const override;
	virtual void PrePipelineMutateCameraState(FCinemachineCameraState& InState, float DeltaTime) override;
	virtual ECVStageType GetStage() const override { return ECVStageType::Aim; }
	virtual void MutateCameraState(FCinemachineCameraState& InState, float DeltaTime) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual float GetMaxDampTime() const override;
	//~ End UCinemachineBaseStage

    //~ Begin ICinemachineModifiableCompositionInterface
	virtual FCinemachineScreenComposerSettings GetComposition() const override
	{
		return Composition;
	}
	virtual void SetComposition(const FCinemachineScreenComposerSettings& InComposition) override
	{
		Composition = InComposition;
	}
    //~ End ICinemachineModifiableCompositionInterface

	FVector GetLookAtPointAndSetTrackedPoint(FVector LookAt, FVector WorldUp, float DeltaTime);

private:
	void RotateToScreenBounds(FCinemachineCameraState& InState, FRect ScreenRect, FVector TrackedLocation, FRotator& RigOrientation, float FOV, float FOVH, float DeltaTime);

	void ClampVerticalBounds(FRect& Rect, FVector Direction, FVector Up, float FOV);

public:
	UPROPERTY(EditDefaultsOnly, Category = Stage)
	FVector TargetOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = Stage)
	FCinemachineLookaheadSettings LookaheadSettings;

	UPROPERTY(EditDefaultsOnly, Category = Stage, meta = (ClampMin = 0.0F), meta = (Tooltip = "수평 축으로 점차 감쇠하는 값"))
	float DampingX = 0.0F;

	UPROPERTY(EditDefaultsOnly, Category = Stage,meta = (ClampMin = 0.0F), meta = (Tooltip = "수직 축으로 점차 감쇠하는 값"))
	float DampingY = 0.0F;

	UPROPERTY()
	FCinemachineScreenComposerSettings Composition;

	UPROPERTY(EditDefaultsOnly, Category = Stage, meta = (DisplayName = "중앙 배치", Tooltip = "이 카메라가 활성화되면 대상을 화면 중앙으로 강제 배치합니다. False면 데드 존의 가장자리에 대상을 고정합니다."))
	bool bCenterOnActivate = true;

	FVector TrackedPoint = FVector::ZeroVector;

private:
	FVector CameraLocationPrevFrame = FVector::ZeroVector;
	
	FVector LookAtPrevFrame = FVector::ZeroVector;

	FVector2D ScreenOffsetPrevFrame = FVector2D::ZeroVector;

	FRotator CameraOrientationPrevFrame = FRotator::ZeroRotator;

	FLocationPredictor Predictor;

	FCVFOVCache FOVCache;
};
