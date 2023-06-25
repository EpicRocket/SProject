

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SceneComponent.h"
#include "CinemachineCameraState.h"
#include "CinemachineCameraInterface.generated.h"

UINTERFACE(BlueprintType)
class UCinemachineCameraInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	virtual FString GetCameraName() const
	{
		return FString(TEXT("Unknown Camera"));
	}

	virtual FString GetDescription() const
	{
		return FString(TEXT("Unknown Description"));
	}

	virtual int32 GetPriority() const
	{
		return 10;
	}

	virtual void SetPriority(int32 InValue) = 0;

	virtual USceneComponent* GetLookAt() const = 0;

	virtual void SetLookAt(USceneComponent* LookAtSceneComponent) = 0;

	virtual USceneComponent* GetFollow() const = 0;

	virtual void SetFollow(USceneComponent* FollowSceneComponent) = 0;

	virtual FCinemachineCameraState GetState() const = 0;

	virtual ICinemachineCameraInterface* GetParentCamera()
	{
		return nullptr;
	}

	virtual bool IsLiveChild(ICinemachineCameraInterface* InCamera, bool DominantChildOnly = false) const
	{
		return false;
	}

	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime)
	{
		// Empty
	}

	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime)
	{
		// Empty
	}

	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* InFromCamera, FVector WorldUp, float DeltaTime)
	{
		// Empty
	}

	virtual void OnTargetObjectWarped(USceneComponent* InTarget, FVector InPositionDelta)
	{
		// Empty
	}
};
