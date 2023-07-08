

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
	const FRichCurve* RichCurve = BlendCurve.GetRichCurveConst();
	if (RichCurve == nullptr || RichCurve->GetNumKeys() < 2 || IsComplete())
	{
		return 1.0F;
	}
	else if(FMath::IsNearlyZero(Duration))
	{
		return 0.0F;
	}
	return FMath::Clamp(RichCurve->Eval(TimeInBlend / Duration), 0.0F, 1.0F);
}

bool UCinemachineBlend::IsComplete() const
{
	return TimeInBlend >= Duration || !(IsValid(CameraA) || IsValid(CameraB));
}

bool UCinemachineBlend::Uses(ICinemachineCameraInterface* ICamera)
{
	if (ICamera == Cast<ICinemachineCameraInterface>(CameraA) || ICamera == Cast<ICinemachineCameraInterface>(CameraB))
	{
		return true;
	}

	UBlendSourceVirtualCamera* Source = Cast<UBlendSourceVirtualCamera>(CameraA);
	if (IsValid(Source) && Source->Blend->Uses(ICamera))
	{
		return true;
	}

	Source = Cast<UBlendSourceVirtualCamera>(CameraB);
	if (IsValid(Source) && Source->Blend->Uses(ICamera))
	{
		return true;
	}

	return false;
}

void UCinemachineBlend::UpdateCameraState(FVector WorldUp, float DeltaTime)
{
	if (IsValid(CameraA))
	{
		ICinemachineCameraInterface* ICamera = Cast<ICinemachineCameraInterface>(CameraA);
		if (ICamera)
		{
			ICamera->UpdateCameraState(WorldUp, DeltaTime);
		}
	}
	if (IsValid(CameraB))
	{
		ICinemachineCameraInterface* ICamera = Cast<ICinemachineCameraInterface>(CameraB);
		if (ICamera)
		{
			ICamera->UpdateCameraState(WorldUp, DeltaTime);
		}
	}
}

FCinemachineCameraState UCinemachineBlend::GetState() const
{
	if (IsValid(CameraA) && IsValid(CameraB))
	{
		ICinemachineCameraInterface* ICameraA = Cast<ICinemachineCameraInterface>(CameraA);
		ICinemachineCameraInterface* ICameraB = Cast<ICinemachineCameraInterface>(CameraB);
		if (ICameraA && ICameraB)
		{
			return FCinemachineCameraState::Lerp(ICameraA->GetState(), ICameraB->GetState(), GetBlendWeight());
		}
	}
	else if (IsValid(CameraA))
	{
		ICinemachineCameraInterface* ICamera = Cast<ICinemachineCameraInterface>(CameraA);
		if (ICamera)
		{
			return ICamera->GetState();
		}
	}
	else if (IsValid(CameraB))
	{
		ICinemachineCameraInterface* ICamera = Cast<ICinemachineCameraInterface>(CameraB);
		if (ICamera)
		{
			return ICamera->GetState();
		}
	}
	return FCinemachineCameraState();
}

FCinemachineBlendDefinition::FCinemachineBlendDefinition()
	: Time(1.0F)
{
}
