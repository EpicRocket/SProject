

#pragma once

#include "Components/ActorComponent.h"
#include "CinemachineBlend.h"
#include "Camera/Interface/CinemachineCameraOverrideStackInterface.h"
#include "Camera/Interface/CinemachineMixerInterface.h"
#include "CinemachineBrainComponent.generated.h"

class UCinemachineBrainComponent;
class UCinemachineVirtualCameraBaseComponent;
class UCameraComponent;
class UCinemachineBlendManager;
class USceneComponent;

/**
 * 가상 카메라 제어 컴포넌트
 * 가상 카메라와 실제 카메라 사이를 연결 하는 역할을 합니다.
 * 브레인을 사용하기 위해서는 UCinemachineCoreSubSystem에 등록해 줘야 합니다.
 * 만약 사용하지 않는다면, UCinemachineCoreSubSystem에서 제거해 줘야 합니다.
*/
UCLASS(Blueprintable, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineBrainComponent : public UActorComponent, public ICinemachineCameraOverrideStackInterface, public ICinemachineCameraInterface, public ICinemachineMixerInterface
{
	GENERATED_BODY()

public:
	UCinemachineBrainComponent();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface
	
	//~ Begin ICinemachineCameraOverrideStackInterface Interface
	virtual int32 SetCameraOverride(int32 OverrideId, int32 Priority, ICinemachineCameraInterface* CameraA, ICinemachineCameraInterface* CameraB, float WeightB, float DeltaTime) override;
	virtual void ReleaseCameraOverride(int32 OverrideId) override;
	virtual FVector DefaultWorldUp() const override;
	//~ End ICinemachineCameraOverrideStackInterface Interface

	//~ Begin ICinemachineCameraInterface Interface
	virtual bool IsCameraValid() const override
	{
		return IsValid(this);
	}

	virtual FString GetCameraName() const override
	{
		return FString(TEXT("BrainComponent(CV)"));
	}

	virtual FCinemachineCameraState GetState() const override
	{
		return CurrentCameraState;
	}
	//~ Begin ICinemachineCameraInterface Interface

	//~ Begin ICinemachineMixerInterface Interface
	virtual bool IsLiveChild(ICinemachineCameraInterface* ICamera, bool bDominantChildOnly = false) const override;
	//~ End ICinemachineMixerInterface Interface

	void Init();

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsInitialized() const
	{
		return bIsInitialized;
	}

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Brain")
	virtual void SetEnable(bool bEnable);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsEnable() const
	{
		return bBrainEnabled;
	}

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	UCameraComponent* GetOutputCamera() const
	{
		return OutputCamera;
	}

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	TScriptInterface<ICinemachineCameraInterface> GetActiveVirtualCamera() const;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Brain")
	void ResetState();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	bool IsBlending() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	UCinemachineBlend* GetActiveBlend() const;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Brain")
	void SetActiveBlend(UCinemachineBlend* Value);

	bool IsLiveInBlend(ICinemachineCameraInterface* ICamera);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Brain")
	void ManualUpdate(float DeltaTime);

protected:
	virtual void OnEnable();

	virtual void OnDisable();

private:
	void UpdateVirtualCameras(float DeltaTime);

	UCinemachineVirtualCameraBaseComponent* TopCameraFromPriorityQueue();

	void ProcessActiveCamera(float DeltaTime);

	void PushStateToCamera(FCinemachineCameraState State);

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Cinemachine)
	USceneComponent* WorldUpOverride = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Cinemachine, meta = (DisplayName = "기본 블렌드"))
	FAlphaBlend DefaultBlend;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Cinemachine)
	FCinemachineCameraState CurrentCameraState;

protected:
	UPROPERTY(EditAnywhere, Category = Cinemachine, meta = (DisplayName = "브레인 활성화"))
	bool bBrainEnabled;

private:
	UPROPERTY(VisibleInstanceOnly, Category = Cinemachine, meta = (AllowPrivateAccess = true))
	UCameraComponent* OutputCamera = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = Cinemachine, meta = (AllowPrivateAccess = true))
	UCameraComponent* TargetOverride = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = Cinemachine, meta = (AllowPrivateAccess = true))
	UCinemachineBlendManager* BlendManager = nullptr;

	bool bCacheBrainEnabled = false;

	bool bIsInitialized = false;

	FVector CachedOutputCameraLocation = FVector::ZeroVector;

	FRotator CachedOutputCameraRotation = FRotator::ZeroRotator;
};
