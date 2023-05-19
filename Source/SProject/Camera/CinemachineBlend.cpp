#include "CinemachineBlend.h"
#include "CinemachineCameraInterface.h"

UCinemachineBlend::UCinemachineBlend()
    : CameraA(nullptr)
    , CameraB(nullptr)
    , TimeInBlend(0.0f)
    , Duration(0.0f)
{
}

UCinemachineBlend::UCinemachineBlend(ICinemachineCameraInterface* InCameraA, ICinemachineCameraInterface* InCameraB, const FRuntimeFloatCurve& Curve, float InTimeInBlend, float InDuration)
    : CameraA(InCameraA)
    , CameraB(InCameraB)
    , BlendCurve(Curve)
    , TimeInBlend(InTimeInBlend)
    , Duration(InDuration)
{
}

float UCinemachineBlend::GetBlendWeight() const
{
	return 0.0F;
}

bool UCinemachineBlend::IsValid() const
{
	// Returns whether the blend is valid or not
	return false;
}

bool UCinemachineBlend::IsComplete() const
{
	// Returns whether the blend is complete or not
	return false;
}

FString UCinemachineBlend::GetDescription() const
{
	// Returns the description of the blend
	return FString();
}

bool UCinemachineBlend::Uses(ICinemachineCameraInterface* ICamera)
{
	// Returns whether the blend uses the specified camera or not
	return false;
}

void UCinemachineBlend::UpdateCameraState(FVector WorldUp, float DeltaTime)
{
	// Updates the camera state
}

FCinemachineCameraState UCinemachineBlend::GetState() const
{
	// Returns the camera state
	return FCinemachineCameraState();
}
