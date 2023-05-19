
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "CinemachineCameraState.h"
#include "CinemachineCameraInterface.generated.h"

UINTERFACE(BlueprintType)
class SPROJECT_API UCinemachineCameraInterface : public UInterface
{
	GENERATED_BODY()
};

class SPROJECT_API ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	virtual FString GetName() const
	{
		return FString(TEXT("Unknown Name"));
	}

	virtual FString GetDescription() const
	{
		return FString(TEXT("Unknown Description"));
	}

	virtual int32 GetPriority() const
	{
		return 10;
	}
	virtual void SetPrioirty(int32 InValue) = 0;

	virtual AActor* GetLookAt() = 0;
	virtual void SetLookAt(AActor* InActor) = 0;

	virtual AActor* GetFollow() = 0;
	virtual void SetFollow(AActor* InActor) = 0;

	virtual FCinemachineCameraState GetState() const = 0;

	virtual ICinemachineCameraInterface* GetParentCamera()
	{
		return nullptr;
	}

	virtual bool IsLiveChild(ICinemachineCameraInterface InCamera, bool DominantChildOnly = false)
	{
		return false;
	}

	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime) = 0;

	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) = 0;

	virtual void OnTransitionFromCamera(ICinemachineCameraInterface InFromCamera, FVector WorldUp, float DeltaTime) = 0;

	virtual void OnTargetObjectWarped(AActor* InTarget, FVector InPositionDelta) = 0;
};
