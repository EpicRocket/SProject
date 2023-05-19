
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CinemachineCameraInterface.h"
#include "CinemachineVirtualCameraComponent.generated.h"

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
	virtual FString GetName() const override
	{
		return GetName();
	}

	virtual int32 GetPriority() const override
	{
		return Priority;
	}

	virtual void SetPrioirty(int32 InValue) override
	{
		Priority = InValue;
	}

	virtual AActor* GetLookAt() override
	{
		return LookAtTarget;
	}
	virtual void SetLookAt(AActor* InActor) override
	{
		LookAtTarget = InActor;
	}

	virtual AActor* GetFollow() override
	{
		return FollowTarget;
	}
	virtual void SetFollow(AActor* InActor) override
	{
		FollowTarget = InActor;
	}

	virtual FCinemachineCameraState GetState() const = 0;

	virtual ICinemachineCameraInterface* GetParentCamera() override
	{
        if(!SlaveStatusUpdated)
        {
            UpdateSlaveStatus();
        }
		return ParentCamera;
	}

    virtual void UpdateCameraState(FVector WorldUp, float DeltaTime) override;

	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) override;

	virtual void OnTransitionFromCamera(ICinemachineCameraInterface InFromCamera, FVector WorldUp, float DeltaTime) override;

	virtual void OnTargetObjectWarped(AActor* InTarget, FVector InPositionDelta) override;
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
