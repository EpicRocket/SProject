

#include "CinemachineBlend.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"

UCinemachineBlend::UCinemachineBlend()
	: CameraA(nullptr)
	, CameraB(nullptr)
	, TimeInBlend(0.0f)
	, Duration(0.0f)
{
}

float UCinemachineBlend::GetBlendWeight() const
{
	if (IsComplete())
	{
		return 1.0F;
	}
	else if(FMath::IsNearlyZero(Duration))
	{
		return 0.0F;
	}
	return FMath::Clamp(FAlphaBlend::AlphaToBlendOption(TimeInBlend / Duration, BlendCurve.GetBlendOption(), BlendCurve.GetCustomCurve()), 0.0F, 1.0F);
}

bool UCinemachineBlend::IsComplete() const
{
	return TimeInBlend >= Duration || !IsCameraValid();
}

void UCinemachineBlend::CopyFrom(const UCinemachineBlend* InOther)
{
	CameraA = InOther->CameraA;
	CameraB = InOther->CameraB;
	BlendCurve = InOther->BlendCurve;
	TimeInBlend = InOther->TimeInBlend;
	Duration = InOther->Duration;
}

void UCinemachineBlend::ClearBlend()
{
	CameraA = nullptr;
	TimeInBlend = 0.0F;
	Duration = 0.0F;
}

bool UCinemachineBlend::Uses(ICinemachineCameraInterface* ICamera)
{
	if (!IsCinemachineCameraValid(ICamera))
	{
		return false;
	}

	UObject* CameraObject = Cast<UObject>(ICamera);
	if (CameraObject == CameraA || CameraObject == CameraB)
	{
		return true;
	}

	UCinemachineNestedBlendSource* Source = Cast<UCinemachineNestedBlendSource>(CameraA);
	if (IsValid(Source) && Source->Blend->Uses(ICamera))
	{
		return true;
	}

	Source = Cast<UCinemachineNestedBlendSource>(CameraB);
	return IsValid(Source) && Source->Blend->Uses(ICamera);
}

void UCinemachineBlend::UpdateCameraState(FVector WorldUp, float DeltaTime)
{
	if (IsValid(CameraA))
	{
		ICinemachineCameraInterface* ICamera = Cast<ICinemachineCameraInterface>(CameraA);
		if (IsCinemachineCameraValid(ICamera))
		{
			ICamera->UpdateCameraState(WorldUp, DeltaTime);
		}
	}
	if (IsValid(CameraB))
	{
		ICinemachineCameraInterface* ICamera = Cast<ICinemachineCameraInterface>(CameraB);
		if (IsCinemachineCameraValid(ICamera))
		{
			ICamera->UpdateCameraState(WorldUp, DeltaTime);
		}
	}
}

FCinemachineCameraState UCinemachineBlend::GetState() const
{
	ICinemachineCameraInterface* ICameraA = IsValid(CameraA) ? Cast<ICinemachineCameraInterface>(CameraA) : nullptr;
	ICinemachineCameraInterface* ICameraB = IsValid(CameraB) ? Cast<ICinemachineCameraInterface>(CameraB) : nullptr;

	if (!IsValid(CameraA) || !ICameraA->IsCameraValid())
	{
		if (!IsValid(CameraB) || !ICameraB->IsCameraValid())
		{
			return FCinemachineCameraState();
		}
		return ICameraB->GetState();
	}
	else if (!IsValid(CameraB) || !ICameraB->IsCameraValid())
	{
		return ICameraA->GetState();
	}
	else
	{
		return FCinemachineCameraState::Lerp(ICameraA->GetState(), ICameraB->GetState(), GetBlendWeight());
	}
}
