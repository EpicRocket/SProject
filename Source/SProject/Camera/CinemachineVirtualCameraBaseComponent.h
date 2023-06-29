

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CinemachineCameraInterface.h"
#include "CinemachineCameraState.h" // include lens settings
#include "CinemachineBlend.h"
#include "CinemachineLensSettings.h"
#include "CinemachineTransitionParameters.h"
#include "CinemachineVirtualCameraBaseComponent.generated.h"

UENUM(BlueprintType)
enum class ECineamchineStandbyUpdateMode : uint8
{
	Nerver,
	Always,
	RoundRobin,
};

class UCinemachineExtension;
class UCinemachineTargetGroupComponent;
class UCinemachineTargetGroupBaseComponent;
class ICinemachineInputAxisProviderInterface;
enum class ECinemachineStage : uint8;

/**
 * 가상 카메라 추상 클래스
 * Unreal Engine 카메라와는 별개로, 가상의 카메라를 정의합니다.
 * 가상 카메라는 실제로 카메라 컴포넌트를 가지고 있지 않습니다.
 * 가상 카메라를 사용하기 위해서는 UCinemachineCoreSubSystem에 등록해 줘야 합니다.
 * 만약 사용하지 않는다면, UCinemachineCoreSubSystem에서 제거해 줘야 합니다.
 */
UCLASS(Abstract, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineVirtualCameraBaseComponent : public USceneComponent, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	UCinemachineVirtualCameraBaseComponent();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UActorComponent Interface
	
	//~ Begin ICinemachineCameraInterface Interface
	virtual FString GetCameraName() const override
	{
		return GetName();
	}

	virtual int32 GetPriority() const override
	{
		return Priority;
	}

	virtual void SetPriority(int32 InValue) override
	{
		Priority = InValue;
	}

	virtual USceneComponent* GetLookAt() const override
	{
		return ResolveLookAt(LookAtTarget);
	}

	virtual void SetLookAt(USceneComponent* LookAtSceneComponent) override
	{
		LookAtTarget = LookAtSceneComponent;
	}

	virtual USceneComponent* GetFollow() const override
	{
		return ResolveFollow(FollowTarget);
	}

	virtual void SetFollow(USceneComponent* FollowSceneComponent) override
	{
		FollowTarget = FollowSceneComponent;
	}

	virtual FCinemachineCameraState GetState() const override
	{
		return VCameraState;
	}

	virtual UCinemachineVirtualCameraBaseComponent* GetParentCamera() override
	{
		if (!SlaveStatusUpdated)
		{
			UpdateSlaveStatus();
		}
		return ParentCamera;
	}

	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime) override;

	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) override
	{
		// Empty
	}

	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime) override;

	virtual void OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta) override;
	//~ End ICinemachineCameraInterface Interface

	//~ Begin Only BP Function
	UFUNCTION(BlueprintPure, Category = Cinemachine)
	FString BP_GetCameraName() const
	{
		return GetCameraName();
	}

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	FString BP_GetDescription() const
	{
		return GetDescription();
	}

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	int32 BP_GetPriority() const
	{
		return GetPriority();
	}

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void BP_SetPriority(int32 InValue)
	{
		SetPriority(InValue);
	}

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	USceneComponent* BP_GetLookAt()
	{
		return GetLookAt();
	}

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void BP_SetLookAt(USceneComponent* LookAtSceneComponent)
	{
		SetLookAt(LookAtSceneComponent);
	}

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	USceneComponent* BP_GetFollow()
	{
		return GetFollow();
	}

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void BP_SetFollow(USceneComponent* FollowSceneComponent)
	{
		SetFollow(FollowSceneComponent);
	}
	//~ End Only BP Function

	virtual bool RequiresUserInput();

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void SetEnable(bool bEnable);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsEnable() const
	{
		return bCameraEnable;
	}

	virtual bool GetPreviousStateIsValid() const
	{
		return PreviousStateIsValid;
	}
	virtual void SetPreviousStateIsValid(bool bValue)
	{
		PreviousStateIsValid = bValue;
	}

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void AddExtension(UCinemachineExtension* Extension);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void RemoveExtension(UCinemachineExtension* Extension);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
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
	void MoveToTopOfPrioritySubqueue();

	virtual void ForceCameraLocation(FVector Location, FRotator Rotation);

	virtual UCinemachineTargetGroupBaseComponent* GetFollowTargetGroup();

	virtual UCinemachineVirtualCameraBaseComponent* GetFollowTargetAsVCamera();

	virtual UCinemachineTargetGroupBaseComponent* GetLookAtTargetGroup();

	virtual UCinemachineVirtualCameraBaseComponent* GetLookAtTargetAsVCamera();

	ICinemachineInputAxisProviderInterface* GetInputAxisProvider() const;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Input")
	virtual void SetInputAxisProvider(TScriptInterface<ICinemachineInputAxisProviderInterface> InsertProvider);

protected:
	virtual void OnEnable();

	virtual void OnDisable();

	void InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime);

	void InvokePrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraBaseComponent* VCamera, FCinemachineCameraState& State, float DeltaTime);

	bool InvokeOnTransitionInExtensions(UCinemachineVirtualCameraBaseComponent* FromVCamera, FVector WorldUp, float DeltaTime);

	void ApplyLocationBlendMethod(FCinemachineCameraState& State, ECinemachineBlendHint BlendHint);

	UCinemachineBlend* CreateBlend(UObject* CameraA, UObject* CameraB, FCinemachineBlendDefinition BlendDefinition, UCinemachineBlend* ActiveBlend);

	FCinemachineCameraState PullStateFromVirtualCamera(FVector WorldUp, FCinemachineLensSettings& Lens);

	void UpdateTargetCache();

private:
	void UpdateSlaveStatus();

	void UpdateVcamPoolStatus();

	void InvalidateCachedTargets();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	int32 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	int32 ActivationId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FollowTargetAttachment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LookAtTargetAttachment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	ECineamchineStandbyUpdateMode StandbyUpdateMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	TArray<UCinemachineExtension*> Extensions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	bool bFollowTargetChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	bool bLookAtTargetChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	FCinemachineLensSettings VCameraLens;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	USceneComponent* LookAtTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	USceneComponent* FollowTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cinemachine)
	FCinemachineCameraState VCameraState;

	ICinemachineInputAxisProviderInterface* InputAxisProvider;

	UPROPERTY(EditAnywhere, Category = Cinemachine)
	bool bCameraEnable;

private:
	bool PreviousStateIsValid = false;

	bool SlaveStatusUpdated = false;

	int32 QueuePriority = MAX_int32;

	float BlendStartPosition = 0.0F;

	UPROPERTY(Transient)
	UCinemachineVirtualCameraBaseComponent* ParentCamera = nullptr;

	UPROPERTY(Transient)
	USceneComponent* CachedFollowTarget = nullptr;

	UPROPERTY(Transient)
	UCinemachineVirtualCameraBaseComponent* CachedFollowTargetVCamera = nullptr;

	UPROPERTY(Transient)
	UCinemachineTargetGroupComponent* CachedFollowTargetGroup = nullptr;

	UPROPERTY(Transient)
	USceneComponent* CachedLookAtTarget = nullptr;

	UPROPERTY(Transient)
	UCinemachineVirtualCameraBaseComponent* CachedLookAtTargetVCamera = nullptr;

	UPROPERTY(Transient)
	UCinemachineTargetGroupComponent* CachedLookAtTargetGroup = nullptr;

	bool bCacheCameraEnable;
};
