
#pragma once

#include "Camera/CinemachineVirtualCameraBaseComponent.h"
#include "Camera/CinemachineTransitionParameters.h"
#include "Camera/CinemachineAxisState.h"
#include "Camera/Component/CinemachineOrbitalTransposerComponent.h"
#include "CinemachineFreeLook.generated.h"

class UCinemachineVirtualCameraComponent;
class UCinemachineFreeLook;

USTRUCT(BlueprintType)
struct FCinemachineFreeLockOrbit
{
	GENERATED_USTRUCT_BODY()

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

UCLASS(Blueprintable, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineFreeLook : public UCinemachineVirtualCameraBaseComponent
{
	GENERATED_BODY()
public:
	static const int32 RIG_COUNT = 3;

public:
	UCinemachineFreeLook();

	//~ Begin USceneComponent Interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End USceneComponent Interface

	//~ Begin UCinemachineVirtualCameraBaseComponent Interface
	virtual bool IsLiveChild(ICinemachineCameraInterface* InCamera, bool DominantChildOnly = false) const override;
	virtual void OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) override;
	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime);
	virtual bool RequiresUserInput() override;
	virtual void SetPreviousStateIsValid(bool bValue) override;
	virtual void SetInputAxisProvider(TScriptInterface<ICinemachineInputAxisProviderInterface> InsertProvider);
	//~ End UCinemachineVirtualCameraBaseComponent Interface

	UFUNCTION(BlueprintPure, Category = "Cinemachine|FreeLock")
	UCinemachineVirtualCameraComponent* GetRig(int32 Index);

	UFUNCTION(BlueprintPure, Category = "Cinemachine|FreeLock")
	bool RigsAreCreated() const
	{
		return Rigs.Num() == UCinemachineFreeLook::RIG_COUNT && Orbitals.Num() == UCinemachineFreeLook::RIG_COUNT;
	}

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|FreeLock")
	FVector GetLocalLocationForCameraFromInput(float Alpha); /*Alpha 값은 0 ~ 1 사이 입니다. TODO: 코멘트 작성*/

protected:
	//~ Begin UCinemachineVirtualCameraBaseComponent Interface
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	//~ End UCinemachineVirtualCameraBaseComponent Interface

private:
	float GetYAxisClosestValue(FVector CameraLocation, FVector Up);

	float SteepestDescent(FVector CameraOffset);

	void InvalidateRigCache()
	{
		Orbitals.Empty();
	}

	bool UpdateRigCache();

	float UpdateXAxisHeading(UCinemachineOrbitalTransposerComponent* Orbital, float DeltaTime, FVector Up);

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
	FCinemachineOrbitalTransposerHeading Heading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	FCinemachineAxisStateRecentering RecenterToTargetHeading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	ECineamchineTransposerBindingMode BindingMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock", meta = (ClampMin = 0.0, ClampMax = 1.0))
	float SplineCurvature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock", EditFixedSize)
	TArray<FCinemachineFreeLockOrbit> Orbits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|FreeLock")
	FRuntimeFloatCurve BlendCurve;

private:
	UPROPERTY(VisibleAnywhere, Transient, Category = "Cinemachine|FreeLock", meta = (AllowPrivateAccess = true))
	TArray<UCinemachineVirtualCameraComponent*> Rigs;

	UPROPERTY(VisibleAnywhere, Transient, Category = "Cinemachine|FreeLock", meta = (AllowPrivateAccess = true))
	TArray<UCinemachineOrbitalTransposerComponent*> Orbitals;

	UPROPERTY(VisibleAnywhere, Transient, Category = "Cinemachine|FreeLock", meta = (AllowPrivateAccess = true))
	bool bDestroyed = false;

	UPROPERTY(Transient)
	UCinemachineBlend* BlendA = nullptr;

	UPROPERTY(Transient)
	UCinemachineBlend* BlendB = nullptr;

	float CachedXAxisHeading = 0.0F;

	uint64 LastHeadingUpdateFrame = 0;

	UPROPERTY(VisibleAnywhere, Category = "Cinemachine|FreeLock", EditFixedSize, meta = (AllowPrivateAccess = true))
	TArray<FCinemachineFreeLockOrbit> CachedOrbits;

	float CachedTension = 0.0F;

	TArray<FVector4> CachedKnots;

	TArray<FVector4> CachedCtrl1;

	TArray<FVector4> CachedCtrl2;
};
