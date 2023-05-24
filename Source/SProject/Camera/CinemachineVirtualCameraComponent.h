
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CinemachineCameraInterface.h"
#include "CinemachineVirtualCameraComponent.generated.h"

UENUM(BlueprintType)
enum class ECineamchineStandbyUpdateMode : uint8
{
	Nerver,
	Always,
	RoundRobin,
};

class AActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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

	virtual ICinemachineCameraInterface* GetParentCamera()
	{
		if (!SlaveStatusUpdated)
		{
			UpdateSlaveStatus();
		}
		return ParentCamera;
	}

	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime)
	{
		// TODO
	}

	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime)
	{
		// TODO
	}

	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* InFromCamera, FVector WorldUp, float DeltaTime)
	{
		// TODO
	}

	virtual void OnTargetObjectWarped(AActor* InTarget, FVector InPositionDelta)
	{
		// TODO
	}
	//~ End ICinemachineCameraInterface Interface

	AActor* ResolveFollow(AActor* LocalFollow);

	AActor* ResolveLookAt(AActor* LocalLookAt);

private:
	void UpdateSlaveStatus();

public:
	int32 Priority = 10;

protected:
	UPROPERTY()
	AActor* LookAtTarget;

	UPROPERTY()
	AActor* FollowTarget;

private:
	bool SlaveStatusUpdated;

	UPROPERTY()
	UCinemachineVirtualCameraComponent* ParentCamera;
};
