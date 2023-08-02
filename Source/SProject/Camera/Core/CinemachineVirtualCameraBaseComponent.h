

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Camera/Interface/CinemachineCameraInterface.h"
#include "CinemachineCameraState.h" // include lens settings
#include "CinemachineVirtualCameraBaseComponent.generated.h"

class USceneComponent;
class UCinemachineExtension;
class UCinemachineBaseStage;
class UCinemachineTargetGroupComponent;
class UCinemachineTargetGroupComponent;
class ICinemachineMixerInterface;
enum class ECVStageType : uint8;

UENUM(BlueprintType)
enum class ECVStandbyUpdateMode : uint8
{
	Nerver UMETA(DisplayName = "카메라 갱신하지 않음"),
	Always UMETA(DisplayName = "카메라 항상 갱신"),
	RoundRobin UMETA(DisplayName = "Tick 당 한번 갱신"),
};

UCLASS(Abstract, BlueprintType, ClassGroup = (Cinemachine))
class UCinemachineVirtualCameraBaseComponent : public USceneComponent, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	UCinemachineVirtualCameraBaseComponent();

	// ~Begin AActor
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	// ~End AActor

	//~ Begin ICinemachineCameraInterface
	virtual bool IsCameraValid() const override
	{
		return IsInitialized();
	}

	virtual FString GetCameraName() const override
    {
		return GetName();
	}

	virtual FCinemachineCameraState GetState() const override
	{
		return CameraState;
	}

	virtual TScriptInterface<ICinemachineMixerInterface> GetParentCamera() override;

	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime) override;
	//~ End ICinemachineCameraInterface

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void Init();

    UFUNCTION(BlueprintCallable, Category = Cinemachine)
    void SetEnable(bool bEnable);

    UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsEnable() const
    {
        return bCameraEnable;
    }

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	virtual USceneComponent* GetFollow() const { return nullptr; /*Needs Overriding*/ }

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void SetFollow(USceneComponent* FollowSceneComponent) { /*Needs Overriding*/ }

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	virtual USceneComponent* GetLookAt() const { return nullptr; /*Needs Overriding*/ }

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void SetLookAt(USceneComponent* LookAtSceneComponent) { /*Needs Overriding*/ }

	UFUNCTION(BlueprintPure, Category = Cinemachine)
    virtual bool GetPreviousStateIsValid() const
	{
		return bPreviousStateIsValid;
	}

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void SetPreviousStateIsValid(bool bValue)
	{
		bPreviousStateIsValid = bValue;
	}

	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) { /*Needs Overriding*/ }

	virtual void OnCameraActivated(FCVActivationEventParams Params);

	virtual void OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation);

	virtual float GetMaxDampTime();

    float DetachedFollowTargetDamp(float Initial, float DampTime, float DeltaTime) const;

	FVector DetachedFollowTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const;

	FVector DetachedFollowTargetDamp(FVector Initial, float DampTime, float DeltaTime) const;

	float DetachedLookAtTargetDamp(float Initial, float DampTime, float DeltaTime) const;

	FVector DetachedLookAtTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const;

	FVector DetachedLookAtTargetDamp(FVector Initial, float DampTime, float DeltaTime) const;

    UFUNCTION(BlueprintPure, Category = Cinemachine)
	USceneComponent* ResolveFollow(USceneComponent* LocalFollow) const;

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	USceneComponent* ResolveLookAt(USceneComponent* LocalLookAt) const;

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void MoveToTopOfPrioritySubQueue();

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void AddExtension(UCinemachineExtension* Extension);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void RemoveExtension(UCinemachineExtension* Extension);

	virtual UCinemachineTargetGroupComponent* GetFollowTargetGroup();

	virtual UCinemachineVirtualCameraBaseComponent* GetFollowTargetAsVCamera();

	virtual UCinemachineTargetGroupComponent* GetLookAtTargetGroup();

	virtual UCinemachineVirtualCameraBaseComponent* GetLookAtTargetAsVCamera();

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void Prioritize()
	{
		UpdateVcamPoolStatus();
	}

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsInitialized() const
	{
		return bIsInitialized;
	}

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsLive();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Blend")
	bool IsParticipatingInBlend();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	virtual UCinemachineBaseStage* GetStage(ECVStageType Stage) { return nullptr; }

protected:
	// ~Begin AActor
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	// ~End AActor

	virtual void OnInitailize() { /* Needs Overrding */ }

    virtual void OnEnable();

	virtual void OnDisable();

	void InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStageType Stage, FCinemachineCameraState& State, float DeltaTime);

	void InvokePrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraBaseComponent* VCamera, FCinemachineCameraState& State, float DeltaTime);

	bool InvokeOnTransitionInExtensions(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime);

	FCinemachineCameraState PullStateFromVirtualCamera(FVector WorldUp, FCinemachineLensSettings& Lens);

	void UpdateTargetCache();

private:
    void UpdateStatusAsChild();

	void UpdateVcamPoolStatus();

	void InvalidateCachedTargets();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cinemachine, meta = (DisplayName = "우선 순위"))
	int32 Priority = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Cinemachine)
	int32 ActivationId = INDEX_NONE;

	float FollowTargetAttachment = 0.0F;

	float LookAtTargetAttachment = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	ECVStandbyUpdateMode StandbyUpdateMode = ECVStandbyUpdateMode::RoundRobin;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Cinemachine)
	TArray<UCinemachineExtension*> Extensions;

	bool bFollowTargetChanged = false;

	bool bLookAtTargetChanged = false;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Cinemachine)
	FCinemachineCameraState CameraState;

	UPROPERTY(EditAnywhere, Category = Cinemachine, meta = (DisplayName = "카메라 활성화"))
	bool bCameraEnable = false;

private:
	bool bIsInitialized = false;

	bool bPreviousStateIsValid = false;

	bool bChildStatusUpdated = false;

	int32 QueuePriority = MAX_int32;

	float BlendStartPosition = 0.0F;

	UPROPERTY()
	UCinemachineVirtualCameraBaseComponent* ParentCamera = nullptr;

	UPROPERTY()
	USceneComponent* CachedFollowTarget = nullptr;

	UPROPERTY()
	UCinemachineVirtualCameraBaseComponent* CachedFollowTargetVCamera = nullptr;

	UPROPERTY()
	UCinemachineTargetGroupComponent* CachedFollowTargetGroup = nullptr;

	UPROPERTY()
	USceneComponent* CachedLookAtTarget = nullptr;

	UPROPERTY()
	UCinemachineVirtualCameraBaseComponent* CachedLookAtTargetVCamera = nullptr;

	UPROPERTY()
	UCinemachineTargetGroupComponent* CachedLookAtTargetGroup = nullptr;

	bool bCacheCameraEnable = false;
};
