

#pragma once

#include "CinemachineCameraInterface.h"
#include "CinemachineCameraOverrideStackInterface.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineCameraOverrideStackInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineCameraOverrideStackInterface
{
	GENERATED_BODY()

public:
	virtual int32 SetCameraOverride(int32 OverrideId, int32 Priority, ICinemachineCameraInterface* CameraA, ICinemachineCameraInterface* CameraB, float WeightB, float DeltaTime) = 0;

	virtual void ReleaseCameraOverride(int32 OverrideId) = 0;

	virtual FVector DefaultWorldUp() const
	{
		return FVector::UpVector;
	}
};
