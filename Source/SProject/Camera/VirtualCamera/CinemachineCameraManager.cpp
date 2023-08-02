

#include "CinemachineCameraManager.h"
#include "Camera/Core/CinemachineBlend.h"
#include "Camera/Core/CinemachineBlendManager.h"
#include "Camera/Core/CinemachineCoreSubSystem.h"

bool UCinemachineCameraManager::IsLiveChild(ICinemachineCameraInterface* ICamera, bool DominantChildOnly) const
{
	if (!IsValid(BlendManager))
	{
		return false;
	}
	return BlendManager->IsLive(ICamera);
}

TScriptInterface<ICinemachineCameraInterface> UCinemachineCameraManager::GetLiveChild()
{
	if(IsValid(BlendManager))
	{
		ICinemachineCameraInterface* LiveCamera = BlendManager->GetActiveVirtualCamera();
		if (!IsCinemachineCameraValid(LiveCamera))
		{
			return nullptr;
		}
		TScriptInterface<ICinemachineCameraInterface> Result;
		Result.SetObject(Cast<UObject>(LiveCamera));
		Result.SetInterface(LiveCamera);
		return Result;
	}
	return nullptr;
}

bool UCinemachineCameraManager::IsBlending() const
{
	if (IsValid(BlendManager))
	{
		return BlendManager->IsBlending();
	}
	return false;
}

UCinemachineBlend* UCinemachineCameraManager::GetActiveBlend() const
{
	if (IsValid(BlendManager))
	{
		return BlendManager->GetActiveBlend();
	}
	return nullptr;
}

TArray<UCinemachineVirtualCameraBaseComponent*> UCinemachineCameraManager::GetChildCameras()
{
	return ChildCameras;
}

void UCinemachineCameraManager::OnInitailize()
{
	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (HasAllFlags(RF_Transient))
	{
		Flags |= RF_Transient;
	}
	BlendManager = NewObject<UCinemachineBlendManager>(this, NAME_None, Flags);
	BlendManager->Init();
}

void UCinemachineCameraManager::OnEnable()
{
	Super::OnEnable();
	if (IsValid(BlendManager))
	{
		BlendManager->SetEnable(true);
		BlendManager->LookupBlend = DefaultBlend;
	}
}

void UCinemachineCameraManager::OnDisable()
{
	Super::OnDisable();
	if (IsValid(BlendManager))
	{
		BlendManager->SetEnable(false);
	}
}

void UCinemachineCameraManager::SetLiveChild(ICinemachineCameraInterface* ActiveCamera, FVector WorldUp, float DeltaTime)
{
	if (IsValid(BlendManager))
	{
		BlendManager->UpdateRootFrame(this, ActiveCamera, WorldUp, DeltaTime);
		BlendManager->ComputeCurrentBlend();
		BlendManager->ProcessActiveCamera(this, WorldUp, DeltaTime);
	}
}

void UCinemachineCameraManager::ResetLiveChild()
{
	if (IsValid(BlendManager))
	{
		BlendManager->ResetRootFrame();
	}
}

void UCinemachineCameraManager::FinalizeCameraState(float DeltaTime)
{
	if (IsValid(BlendManager) && IsValid(BlendManager->CurrentLiveCameras))
	{
		CameraState = BlendManager->CurrentLiveCameras->GetState();
	}
	InvokePostPipelineStageCallback(this, ECVStageType::Finalize, CameraState, DeltaTime);
}

USceneComponent* UCinemachineCameraManager::GetFollow() const
{	
	return ResolveFollow(TrackingTarget);
}

void UCinemachineCameraManager::SetFollow(USceneComponent* FollowSceneComponent)
{
	TrackingTarget = FollowSceneComponent;
}

USceneComponent* UCinemachineCameraManager::GetLookAt() const
{
	return ResolveLookAt(IsValid(LookAtTarget) ? LookAtTarget : TrackingTarget);
}

void UCinemachineCameraManager::SetLookAt(USceneComponent* LookAtSceneComponent)
{
	LookAtTarget = LookAtSceneComponent;
}

void UCinemachineCameraManager::SetPreviousStateIsValid(bool bValue)
{
	Super::SetPreviousStateIsValid(true);
	for (UCinemachineVirtualCameraBaseComponent* VCamera : ChildCameras)
	{
		if (IsValid(VCamera))
		{
			VCamera->SetPreviousStateIsValid(bValue);
		}
	}
}

void UCinemachineCameraManager::InternalUpdateCameraState(FVector WorldUp, float DeltaTime)
{
	if (!IsCameraValid())
	{
		return;
	}

	if (!GetPreviousStateIsValid())
	{
		ResetLiveChild();
	}

	UCinemachineVirtualCameraBaseComponent* SelectedVCamera = ChooseCurrentCamera(WorldUp, DeltaTime);
	if (IsValid(SelectedVCamera) && !SelectedVCamera->IsEnable())
	{
		SelectedVCamera->SetEnable(true);
		SelectedVCamera->UpdateCameraState(WorldUp, DeltaTime);
	}
	SetLiveChild(SelectedVCamera, WorldUp, DeltaTime);

	TScriptInterface<ICinemachineCameraInterface> LiveCamera = GetLiveChild();
	if (IsCinemachineCameraValid(TransitionFrom) && !IsBlending() && IsValid(LiveCamera.GetObject()))
	{
		FCVActivationEventParams Params;
		Params.Origin.SetObject(this);
		Params.Origin.SetInterface(this);
		Params.OutgoingCamera.SetObject(Cast<UObject>(TransitionFrom));
		Params.OutgoingCamera.SetInterface(TransitionFrom);
		Params.IncomingCamera = LiveCamera;
		Params.bCut = false;
		Params.WorldUp = WorldUp;
		Params.DeltaTime = DeltaTime;
		LiveCamera->OnCameraActivated(Params);
	}

	FinalizeCameraState(DeltaTime);
	TransitionFrom = nullptr;
	SetPreviousStateIsValid(true);

}

void UCinemachineCameraManager::OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime)
{
	Super::OnTransitionFromCamera(FromCamera, WorldUp, DeltaTime);
	TransitionFrom = FromCamera.GetInterface();
	InvokeOnTransitionInExtensions(FromCamera, WorldUp, DeltaTime);
	InternalUpdateCameraState(WorldUp, DeltaTime);
}

void UCinemachineCameraManager::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	for (UCinemachineVirtualCameraBaseComponent* VCamera : ChildCameras)
	{
		if (IsValid(VCamera))
		{
			VCamera->ForceCameraLocation(Location, Rotation);
		}
	}
	Super::ForceCameraLocation(Location, Rotation);
}
