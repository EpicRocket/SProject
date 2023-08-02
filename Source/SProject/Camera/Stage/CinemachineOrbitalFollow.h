

#pragma once

#include "Camera/Core/CinemachineBaseStage.h"
#include "Camera/Core/CinemachineTargetTracking.h"
#include "Camera/Core/CinemachineInputAxis.h"
#include "Camera/Interface/CinemachineModifierInterface.h"
#include "Camera/Interface/CinemachineInputAxisInterface.h"
#include "CinemachineOrbitalFollow.generated.h"

UENUM(BlueprintType)
enum class ECVOrbitStyle : uint8
{
    Sphere      UMETA(Tooltip = "카메라는 대상과 고정된 거리에 있으며 구를 정의합니다."),
    ThreeRing   UMETA(Tooltip = "카메라 표면은 대상 주위에 3개의 원형 궤도를 연결하는 선을 돌출하여 만듭니다.")
};

UENUM(BlueprintType)
enum class ECVOrbitalFollowReferenceFrame : uint8
{
	AxisCenter		UMETA(ToolTip = "정적 기준 프레임. 축 중심값은 동적으로 업데이트되지 않습니다."),
	TrackingTarget	UMETA(ToolTip = "축 중심이 추적 대상의 전방 뒤쪽에 있도록 동적으로 조정됩니다. 추적 대상의 전방 뒤에 동적으로 조정됩니다."),
	LookAtTarget	UMETA(ToolTip = "축 중심은 LookAt Target의 전방 뒤에 오도록 동적으로 조정됩니다."),
};

USTRUCT(BlueprintType)
struct FCVOrbit
{
	GENERATED_BODY()

public:
	FCVOrbit() = default;
	FCVOrbit(float InRadius, float InHeight) : Radius(InRadius), Height(InHeight) {}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "범위"))
	float Radius = 300.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "높이"))
	float Height = 150.0F;
};

USTRUCT(BlueprintType)
struct FCVOrbitRigsSettings
{
	GENERATED_BODY()

public:
	FCVOrbitRigsSettings()
		: Top(FCVOrbit(600.0F, 300.0F))
		, Center(FCVOrbit(300.0F, 150.0F))
		, Bottom(FCVOrbit(150.0F, 75.0F))
	{}

	bool operator==(const FCVOrbitRigsSettings& Other) const
	{
		return Top.Radius == Other.Top.Radius && Top.Height == Other.Top.Height &&
			Center.Radius == Other.Center.Radius && Center.Height == Other.Center.Height &&
			Bottom.Radius == Other.Bottom.Radius && Bottom.Height == Other.Bottom.Height &&
			SplineCurvature == Other.SplineCurvature;
	}

	bool operator!=(const FCVOrbitRigsSettings& Other) const
	{
		return Top.Radius != Other.Top.Radius || Top.Height != Other.Top.Height ||
			Center.Radius != Other.Center.Radius || Center.Height != Other.Center.Height ||
			Bottom.Radius != Other.Bottom.Radius || Bottom.Height != Other.Bottom.Height ||
			SplineCurvature != Other.SplineCurvature;
	}

	void operator=(const FCVOrbitRigsSettings& Other)
	{
		Top = Other.Top;
		Center = Other.Center;
		Bottom = Other.Bottom;
		SplineCurvature = Other.SplineCurvature;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCVOrbit Top;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCVOrbit Center;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCVOrbit Bottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0))
	float SplineCurvature = 0.5F;
};

USTRUCT()
struct FCVOrbitSplineCache
{
	GENERATED_BODY()

public:
	FCVOrbitSplineCache();
	void UpdateOrbitCache(const FCVOrbitRigsSettings& Orbits);
	FVector4 SplineValue(float Alpha) const;

public:
	FCVOrbitRigsSettings Setting;
	TArray<FVector4> CachedKnots;
	TArray<FVector4> CachedCtrl1;
	TArray<FVector4> CachedCtrl2;
};

UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories, ClassGroup = (Cinemachine))
class UCinemachineOrbitalFollow
	: public UCinemachineBaseStage
	, public ICinemachineInputAxisInterface
	, public ICinemachineModifierValueSourceInterface
	, public ICinemachineModifiableLocationDampingInterface
	, public ICinemachineModifiableDistanceInterface
{
	GENERATED_BODY()

public:
	UCinemachineOrbitalFollow();

	//~ Begin UCinemachineBaseStage
	virtual void Validate() override;
	virtual bool IsStageValid() const override;
	virtual ECVStageType GetStage() const override { return ECVStageType::Body; }
	virtual void MutateCameraState(FCinemachineCameraState& InState, float DeltaTime) override;
	virtual bool OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual float GetMaxDampTime() const override;
	//~ End UCinemachineBaseStage

	//~ Begin ICinemachineInputAxisInterface
	virtual void ProcessInput(FGameplayTag InutAxisTag, float InputValue, float DeltaTime) override;
	virtual void ResetInput(FGameplayTagContainer InputAxisTagContainer) override;
	virtual void OnCancelInputRecentering(FGameplayTag InputAxisTag) override;
	//~ End ICinemachineInputAxisInterface

	//~ Begin ICinemachineModifierValueSourceInterface
	virtual float NormalizedModifierValue() override;
	//~ End ICinemachineModifierValueSourceInterface

	//~ Begin ICinemachineModifiableLocationDampingInterface
	virtual FVector GetLocationDamping() const override;
	virtual void SetLocationDamping(const FVector& InLocationDamping) override;
	//~ End ICinemachineModifiableLocationDampingInterface

	//~ Begin ICinemachineModifiableDistanceInterface
	virtual float GetDistance() const override;
	virtual void SetDistance(float InDistance) override;
	//~ End ICinemachineModifiableDistanceInterface

	FVector GetCameraOffsetForNormalizedAxisValue(float Alpha) const;

	FVector4 GetCameraPoint();

	FRotator GetReferenceOrientation() const;

protected:
	//~ Begin UCinemachineBaseStage
	virtual void OnInitialize() override;
	//~ End UCinemachineBaseStage

private:
	void InferAxesFromLocationSphere(FVector Direction, float Distance);

	void InferAxesFromLocationThreeRing(FVector Direction, float Distance);

	float GetHorizontalAxis(FVector Direction, FQuat Rotation, FVector Up) const;

	float GetVerticalAxisClosestValue(FVector Direction, float Distance, FVector Up, FVector& SplinePoint);

	float GetSteepestDescent(FVector CameraOffset);

	float GetAngle(FVector CameraOffset, float Input) const;

	/** 대칭 미분 지수를 사용한 미분 근사화(유한 차분) */
	float GetSlopeOfAngle(FVector CameraOffset, float Input) const;

	/** 포인트에 가장 가까운 선(근사 스플라인)을 기반으로 한 초기 추측  */
	float GetInitialGuess(FVector CameraLocationInRigSpace);

	void UpdateHorizontalCenter(FRotator ReferenceOrientation);

public:
	UPROPERTY(EditDefaultsOnly, Category = Stage, meta = (DisplayName = "타겟 추적 설정"))
	FCinemachineTrackerSettings TrackerSettings;

	UPROPERTY(EditDefaultsOnly, Category = Stage, meta = (DisplayName = "궤도 타입"))
	ECVOrbitStyle OrbitStyle = ECVOrbitStyle::Sphere;

	UPROPERTY(EditDefaultsOnly, Category = Stage, meta = (DisplayName = "범위", EditCondition = "OrbitStyle == ECVOrbitStyle::Sphere", EditConditionHides, ClampMin = 0.0))
	float Radius = 600.0F;

	UPROPERTY(EditDefaultsOnly, Category = Stage, meta = (DisplayName = "궤도 설정", EditCondition = "OrbitStyle == ECVOrbitStyle::ThreeRing", EditConditionHides))
	FCVOrbitRigsSettings Orbits;

	UPROPERTY(EditDefaultsOnly, Category = Stage, meta = (DisplayName = "업데이트 대상 정의"))
	ECVOrbitalFollowReferenceFrame RecenteringTarget = ECVOrbitalFollowReferenceFrame::TrackingTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (DisplayName = "수평 축 중심 정의"))
	FCinemachineInputAxis HorizontalAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (DisplayName = "수직 축 중심 정의"))
	FCinemachineInputAxis VerticalAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (DisplayName = "확대/축소 축 정의"))
	FCinemachineInputAxis RadialAxis;

private:
	FVector PreviousOffset = FVector::ZeroVector;

	UPROPERTY()
	FCinemachineTracker TargetTracker;

	FCVOrbitSplineCache OrbitCache;

	FCinemachineInputAxisResetEvent ResetHandler;
};
