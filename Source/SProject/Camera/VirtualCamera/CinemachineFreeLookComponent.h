

#pragma once

#include "Camera/CinemachineVirtualCameraBaseComponent.h"
#include "Camera/CinemachineTransitionParameters.h"
#include "Camera/CinemachineAxisState.h"
#include "Camera/Component/CinemachineOrbitalTransposer.h"
#include "CinemachineFreeLookComponent.generated.h"

class UCinemachineVirtualCameraComponent;

USTRUCT(BlueprintType)
struct FCVFreeLookTransposerDamp
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float X = 1.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Y = 1.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Z = 1.0F;
};

USTRUCT(BlueprintType)
struct FCinemachineFreeLockOrbit
{
	GENERATED_BODY()

public:
	FCinemachineFreeLockOrbit()
		: Height(0.f)
		, Radius(0.f)
	{
	}
	FCinemachineFreeLockOrbit(float InHeight, float InRadius)
		: Height(InHeight)
		, Radius(InRadius)
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine/FreeLock")
	float Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine/FreeLock")
	float Radius;
};

UENUM(BlueprintType)
enum class ECVFreeLookRigType : uint8
{
	TopRig,
	MiddleRig,
	BottomRig,
};

UCLASS(BlueprintType, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineFreeLookComponent : public UCinemachineVirtualCameraBaseComponent
{
	GENERATED_BODY()
public:
	static const int32 RIG_COUNT = 3;

public:
	UCinemachineFreeLookComponent();

	//~ Begin USceneComponent Interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	//~ End USceneComponent Interface

	//~ Begin UCinemachineVirtualCameraBaseComponent Interface
	virtual bool IsLiveChild(ICinemachineCameraInterface* InCamera, bool DominantChildOnly = false) const override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) override;
	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime);
	virtual bool RequiresUserInput() override;
	virtual void SetPreviousStateIsValid(bool bValue) override;
	virtual void UpdateInputAxisProvider();
	//~ End UCinemachineVirtualCameraBaseComponent Interface

	UFUNCTION(BlueprintPure, Category = "Cinemachine|FreeLock")
	UCinemachineVirtualCameraComponent* GetRig(ECVFreeLookRigType RigType);

	UFUNCTION(BlueprintPure, Category = "Cinemachine|FreeLock")
	FVector GetLocalLocationForCameraFromInput(float Alpha); /*Alpha 값은 0 ~ 1 사이 입니다. TODO: 코멘트 작성*/

protected:
	//~ Begin UCinemachineVirtualCameraBaseComponent Interface
	virtual void OnInitailize_Implementation() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	//~ End UCinemachineVirtualCameraBaseComponent Interface

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|FreeLock")
	void SetRig(ECVFreeLookRigType RigType, UCinemachineVirtualCameraComponent* Rig);

private:
	void SetOrbital(ECVFreeLookRigType RigType, UCinemachineOrbitalTransposer* Orbital);

	float GetYAxisClosestValue(FVector CameraLocation, FVector Up);

	float SteepestDescent(FVector CameraOffset);

	void InvalidateRigCache()
	{
		Orbitals.Empty();
	}

	float UpdateXAxisHeading(UCinemachineOrbitalTransposer* Orbital, float DeltaTime, FVector Up);

	void PushSettingsToRigs();

	float GetYAxisValue() const;

	FCinemachineCameraState CalculateNewState(FVector WorldUp, float DeltaTime);

	void UpdateCachedSpline();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	bool bCommonLens;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock", meta = (EditCondition = "!bCommonLens"))
	FCinemachineLensSettings CommonLens;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	FCinemachineTransitionParameters TransitionParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	FCinemachineAxisState YAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	FCinemachineAxisStateRecentering YAxisRecentering;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	FCinemachineAxisState XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	FCVOrbitalTransposerHeading Heading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	FCinemachineAxisStateRecentering RecenterToTargetHeading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	ECVBindingMode BindingMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock", meta = (ClampMin = 0.0, ClampMax = 1.0))
	float SplineCurvature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock", EditFixedSize)
	TMap<ECVFreeLookRigType, FCinemachineFreeLockOrbit> Orbits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	FRuntimeFloatCurve BlendCurve;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Cinemachine|FreeLock", EditFixedSize)
	TMap<ECVFreeLookRigType, UCinemachineVirtualCameraComponent*> Rigs;

	UPROPERTY(VisibleAnywhere, Category = "Cinemachine|FreeLock", EditFixedSize)
	TMap<ECVFreeLookRigType, UCinemachineOrbitalTransposer*> Orbitals;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true", ForceRebuildProperty = "TopRigAimStageTemplate"))
	TSubclassOf<UCinemachineBaseStage> TopRigAimStageClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true", ForceRebuildProperty = "MiddleRigAimStageTemplate"))
	TSubclassOf<UCinemachineBaseStage> MiddleRigAimStageClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true", ForceRebuildProperty = "BottomRigAimStageTemplate"))
	TSubclassOf<UCinemachineBaseStage> BottomRigAimStageClass;

	UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = "Cinemachine|Spawn", meta = (ShowInnerProperties))
	TObjectPtr<UCinemachineBaseStage> TopRigAimStageTemplate;

	UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = "Cinemachine|Spawn", meta = (ShowInnerProperties))
	TObjectPtr<UCinemachineBaseStage> MiddleRigAimStageTemplate;

	UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = "Cinemachine|Spawn", meta = (ShowInnerProperties))
	TObjectPtr<UCinemachineBaseStage> BottomRigAimStageTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true"))
	FCVFreeLookTransposerDamp TopRigTransposerDamp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true"))
	FCVFreeLookTransposerDamp MiddleRigTransposerDamp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true"))
	FCVFreeLookTransposerDamp BottomRigTransposerDamp;

	bool bDestroyed = false;

	UPROPERTY(Transient)
	UCinemachineBlend* BlendA = nullptr;

	UPROPERTY(Transient)
	UCinemachineBlend* BlendB = nullptr;

	float CachedXAxisHeading = 0.0F;

	uint64 LastHeadingUpdateFrame = 0;

	UPROPERTY(EditFixedSize)
	TMap<ECVFreeLookRigType,FCinemachineFreeLockOrbit> CachedOrbits;

	float CachedTension = 0.0F;

	TArray<FVector4> CachedKnots;

	TArray<FVector4> CachedCtrl1;

	TArray<FVector4> CachedCtrl2;
};
