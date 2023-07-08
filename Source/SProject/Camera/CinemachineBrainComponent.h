

#pragma once

#include "Components/SceneComponent.h"
#include "CinemachineTransitionParameters.h"
#include "CinemachineBlend.h"
#include "CinemachineBrainComponent.generated.h"

class UCinemachineBrainComponent;
class UCinemachineVirtualCameraBaseComponent;
class UCameraComponent;
class ICinemachineCameraInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCinemachineBrainEvent, UCinemachineBrainComponent*, Brain);

USTRUCT()
struct FCVBrainFrame
{
	GENERATED_BODY()

public:
	FCVBrainFrame() = default;
	FCVBrainFrame(UObject* Outer);

public:
	int32 Id = INDEX_NONE;

	UPROPERTY(Transient)
	UCinemachineBlend* Blend = nullptr;

	bool bActive = false;

	UPROPERTY(Transient)
	UCinemachineBlend* WorkingBlend = nullptr;

	UPROPERTY(Transient)
	UBlendSourceVirtualCamera* WorkingBlendSource = nullptr;

	float BlendStartPosition = 0.0F;
};

/**
 * 가상 카메라 제어 컴포넌트
 * 가상 카메라와 실제 카메라 사이를 연결 하는 역할을 합니다.
 * 브레인을 사용하기 위해서는 UCinemachineCoreSubSystem에 등록해 줘야 합니다.
 * 만약 사용하지 않는다면, UCinemachineCoreSubSystem에서 제거해 줘야 합니다.
*/
UCLASS(Blueprintable, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineBrainComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UCinemachineBrainComponent();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UActorComponent Interface
	
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
	UCameraComponent* GetOutputCamera();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	FVector DefaultWorldUp() const;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Brain")
	void ManualUpdate(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	UCinemachineVirtualCameraBaseComponent* GetActiveVirtualCamera();

	bool IsLiveInBlend(ICinemachineCameraInterface* ICamera);

	bool IsLive(ICinemachineCameraInterface* ICamera, bool DominantChildOnly = false);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Brain")
	void ComputeCurrentBlend(UCinemachineBlend* OutputBlend, int32 NumTopLayersToExclude);

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	bool IsBlending() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	UCinemachineBlend* GetActiveBlend() const;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Brain")
	void SetActiveBlend(UCinemachineBlend* Value);

protected:
	virtual void OnInitailize();

	virtual void OnEnable();

	virtual void OnDisable();

private:
	void UpdateVirtualCameras(float DeltaTime);

	int32 GetBrainFrame(int32 WithId);

	void ProcessActiveCamera(float DeltaTime);

	void UpdateFrame0(float DeltaTime);

	UCinemachineVirtualCameraBaseComponent* TopCameraFromPriorityQueue();

	FCinemachineBlendDefinition LookupBlend(UObject* FromKey, UObject* ToKey);

	void PushStateToCamera(FCinemachineCameraState State);

public:
	UPROPERTY(BlueprintAssignable, Category = "Cinemachine|Brain")
	FCinemachineBrainEvent CameraCutEvent;

	UPROPERTY(BlueprintAssignable, Category = "Cinemachine|Brain")
	FCinemachineVirtualCameraActivatedEvent CameraActivatedEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Brain")
	USceneComponent* WorldUpOverride = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Brain")
	FCinemachineBlendDefinition DefaultBlend;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cinemachine|Brain")
	FCinemachineCameraState CurrentCameraState;

protected:
	UPROPERTY(EditAnywhere, Category = "Cinemachine|Brain")
	bool bBrainEnabled;

private:
	UPROPERTY(VisibleAnywhere, Category = "Cinemachine|Brain", meta = (AllowPrivateAccess = true))
	UCameraComponent* OutputCamera = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Cinemachine|Brain", meta = (AllowPrivateAccess = true))
	UCameraComponent* TargetOverride = nullptr;

	uint64 LastFrameUpdated = 0;

	UPROPERTY(Transient)
	TArray<FCVBrainFrame> FrameStack;

	int32 NextFrameId = 1;

	UPROPERTY(EditAnywhere, Category = "Cinemachine|Brain", meta = (AllowPrivateAccess = true))
	UCinemachineBlend* CurrentLiveCameras = nullptr;

	UCinemachineVirtualCameraBaseComponent* ActiveCameraPreviousFrame = nullptr;

	bool bCacheBrainEnabled = false;

	bool bIsInitialized = false;

	FVector CachedOutputCameraLocation = FVector::ZeroVector;

	FRotator CachedOutputCameraRotation = FRotator::ZeroRotator;
};
