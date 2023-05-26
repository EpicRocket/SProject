
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CinemachineCameraInterface.h"
#include "CinemachineCameraState.h"	// include lens settings
#include "CinemachineBlend.h"
#include "CinemachineVirtualCameraComponent.generated.h"

UENUM(BlueprintType)
enum class ECineamchineStandbyUpdateMode : uint8
{
	Nerver,
	Always,
	RoundRobin,
};

UENUM(BlueprintType)
enum class ECinemachineBlendHint : uint8
{
	None,
	SphericalLocation,
	CylindriacalLocation,
	ScreenSpaceAimWhenTargetsDiffer
};

class AActor;
class UCinemachineExtension;
class UCinemachineTargetGroupComponent;
enum class ECinemachineStage : uint8;

UCLASS(Abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPROJECT_API UCinemachineVirtualCameraComponent : public UCameraComponent, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	UCinemachineVirtualCameraComponent();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	//~ Begin ICinemachineCameraInterface Interface
	virtual FString GetCameraName() const
	{
		return GetName();
	}

	virtual int32 GetPriority() const
	{
		return Priority;
	}

	virtual void SetPriority(int32 InValue)
	{
		Priority = InValue;
	}

	virtual AActor* GetLookAt()
	{
		return LookAtTarget;
	}
	virtual void SetLookAt(AActor* InActor)
	{
		LookAtTarget = InActor;
	}

	virtual AActor* GetFollow()
	{
		return FollowTarget;
	}
	virtual void SetFollow(AActor* InActor)
	{
		FollowTarget = InActor;
	}

	virtual FCinemachineCameraState GetState()
	{
		return FCinemachineCameraState();
	}

	virtual UCinemachineVirtualCameraComponent* GetParentCamera()
	{
		if (!SlaveStatusUpdated)
		{
			UpdateSlaveStatus();
		}
		return ParentCamera;
	}

	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime);

	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime)
	{
		// Empty
	}

	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* InFromCamera, FVector WorldUp, float DeltaTime);

	virtual void OnTargetObjectWarped(AActor* InTarget, FVector InPositionDelta);
	//~ End ICinemachineCameraInterface Interface

	virtual bool GetPreviousStateIsValid() const
	{
		return false;
	}
	virtual void SetPreviousStateIsValid(bool bValue)
	{
	}

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void AddExtension(UCinemachineExtension* Extension);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void RemoveExtension(UCinemachineExtension* Extension);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	virtual float GetMaxDampTime() const;

	float DetachedFollowTargetDamp(float Initial, float DampTime, float DeltaTime) const;

	FVector DetachedFollowTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const;

	FVector DetachedFollowTargetDamp(FVector Initial, float DampTime, float DeltaTime) const;

	float DetachedLookAtTargetDamp(float Initial, float DampTime, float DeltaTime) const;

	FVector DetachedLookAtTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const;

	FVector DetachedLookAtTargetDamp(FVector Initial, float DampTime, float DeltaTime) const;
	
	UFUNCTION(BlueprintPure, Category = Cinemachine)
	AActor* ResolveFollow(AActor* LocalFollow);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	AActor* ResolveLookAt(AActor* LocalLookAt);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void MoveToTopOfPrioritySubqueue();

	virtual void ForceCameraPosition(FVector Location, FQuat Rotation);

	virtual UCinemachineTargetGroupBaseComponent* GetFollowTargetGroup();

	virtual UCinemachineVirtualCameraComponent* GetFollowTargetAsVCamera();

	virtual UCinemachineTargetGroupBaseComponent* GetLookAtTargetGroup();

	virtual UCinemachineVirtualCameraComponent* GetLookAtTargetAsVCamera();

protected:
	void InvokePostPipelineStageCallback(UCinemachineVirtualCameraComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime);

	void InvokePrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraComponent* VCamera, FCinemachineCameraState& State, float DeltaTime);

	bool InvokeOnTransitionInExtensions(UCinemachineVirtualCameraComponent* FromVCamera, FVector WorldUp, float DeltaTime);

	void ApplyLocationBlendMethod(FCinemachineCameraState& State, ECinemachineBlendHint BlendHint);

	UCinemachineBlend* CreateBlend(UObject* CameraA, UObject* CameraB, FCinemachineBlendDefinition BlendDefinition, UCinemachineBlend* ActiveBlend);

	FCinemachineCameraState PullStateFromVirtualCamera(FVector WorldUp, FCinemachineLensSettings& Lens);

	void UpdateTargetCache();

private:
	void UpdateSlaveStatus();

	void UpdateVCameraStatus();

	void InvalidateCachedTargets();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	int32 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	int32 ActivationId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	float FollowTargetAttachment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	float LootAtTargetAttachment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	ECineamchineStandbyUpdateMode StandbyUpdateMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	TArray<UCinemachineExtension*> Extensions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	bool bFollowTargetChanged;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	bool bLookAtTargetChanged;

protected:
	UPROPERTY()
	AActor* LookAtTarget;

	UPROPERTY()
	AActor* FollowTarget;

private:
	bool SlaveStatusUpdated;

	int32 QueuePriority;

	float BlendStartPosition;

	UPROPERTY()
	UCinemachineVirtualCameraComponent* ParentCamera;

	UPROPERTY()
	AActor* CachedFollowTarget;

	UPROPERTY()
	UCinemachineVirtualCameraComponent* CachedFollowTargetVCamera;

	UPROPERTY()
	UCinemachineTargetGroupComponent* CachedFollowTargetGroup;

	UPROPERTY()
	AActor* CachedLookAtTarget;

	UPROPERTY()
	UCinemachineVirtualCameraComponent* CachedLookAtTargetVCamera;

	UPROPERTY()
	UCinemachineTargetGroupComponent* CachedLookAtTargetGroup;
};
