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
	return FMath::Clamp(RichCurve->Eval(TimeInBlend / Duration), 0.0F, 1.0F);
}

bool UCinemachineBlend::IsComplete() const
{
	return TimeInBlend >= Duration || !(IsValid(CameraA) || IsValid(CameraB));
}

FString UCinemachineBlend::GetDescription() const
{
	TScriptInterface<ICinemachineCameraInterface> ICameraA(CameraA);
	TScriptInterface<ICinemachineCameraInterface> ICameraB(CameraB);
	FString CameraAName = !IsValid(CameraA) && ICameraA ? FString(TEXT("[None]")) : FString::Printf(TEXT("[{0}]"), *ICameraA->GetCameraName());
	FString CameraBName = !IsValid(CameraB) && ICameraB ? FString(TEXT("[None]")) : FString::Printf(TEXT("[{0}]"), *ICameraB->GetCameraName());
	return FString::Printf(TEXT("{0} {1}% from {2} }"), *CameraBName, static_cast<int32>(GetBlendWeight() * 100.0F), *CameraAName);
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
		TScriptInterface<ICinemachineCameraInterface> ICamera(CameraA);
		if (ICamera)
		{
			ICamera->UpdateCameraState(WorldUp, DeltaTime);
		}
	}
	if (IsValid(CameraB))
	{
		TScriptInterface<ICinemachineCameraInterface> ICamera(CameraB);
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
		TScriptInterface<ICinemachineCameraInterface> ICameraA(CameraA);
		TScriptInterface<ICinemachineCameraInterface> ICameraB(CameraB);
		if (ICameraA && ICameraB)
		{
			return FCinemachineCameraState::Lerp(ICameraA->GetState(), ICameraB->GetState(), GetBlendWeight());
		}
	}
	else if (IsValid(CameraA))
	{
		TScriptInterface<ICinemachineCameraInterface> ICamera(CameraA);
		if (ICamera)
		{
			return ICamera->GetState();
		}
	}
	else if (IsValid(CameraB))
	{
		TScriptInterface<ICinemachineCameraInterface> ICamera(CameraB);
		if (ICamera)
		{
			return ICamera->GetState();
		}
	}
	return FCinemachineCameraState();
}

FCinemachineBlendDefinition::FCinemachineBlendDefinition()
	: Time(0.0F)
{
}
