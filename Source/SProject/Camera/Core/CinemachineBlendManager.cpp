

#include "CinemachineBlendManager.h"
#include "CinemachineCoreSubSystem.h"

#define LOCTEXT_NAMESPACE "CinemachineBlendManager"
static ICinemachineCameraInterface* GetDeepCameraBFromBlend(UCinemachineBlend* Blend)
{
	ICinemachineCameraInterface* ICamera = IsValid(Blend) ? ICamera = Cast<ICinemachineCameraInterface>(Blend->CameraB) : nullptr;

	while (UCinemachineNestedBlendSource* NestedBlend = Cast<UCinemachineNestedBlendSource>(ICamera))
	{
		if (!NestedBlend->IsCameraValid())
		{
			break;
		}
		ICamera = Cast<UCinemachineNestedBlendSource>(NestedBlend->Blend->CameraB);
	}
	if (ICamera != nullptr)
	{
		return ICamera;
	}
	return nullptr;
}

static void CollectLiveCameras(UCinemachineBlend* Blend, TArray<UObject*>& OutCameras)
{
	if (IsValid(Blend))
	{
		if (UCinemachineNestedBlendSource* CinemachineNestedBlendSource = Cast<UCinemachineNestedBlendSource>(Blend->CameraA))
		{
			if (IsValid(CinemachineNestedBlendSource->Blend))
			{
				CollectLiveCameras(CinemachineNestedBlendSource->Blend, OutCameras);
			}
			else if (IsValid(Blend->CameraA))
			{
				OutCameras.Emplace(Blend->CameraA);
			}
		}

		if (UCinemachineNestedBlendSource* CinemachineNestedBlendSource = Cast<UCinemachineNestedBlendSource>(Blend->CameraB))
		{
			if (IsValid(CinemachineNestedBlendSource->Blend))
			{
				CollectLiveCameras(CinemachineNestedBlendSource->Blend, OutCameras);
			}
			else if (IsValid(Blend->CameraB))
			{
				OutCameras.Emplace(Blend->CameraB);
			}
		}
	}
}
#undef LOCTEXT_NAMESPACE

ICinemachineCameraInterface* UCinemachineBlendManager::GetActiveVirtualCamera() const
{
	return GetDeepCameraBFromBlend(CurrentLiveCameras);
}

UCinemachineBlend* UCinemachineBlendManager::GetActiveBlend() const
{
	return !IsValid(CurrentLiveCameras) || CurrentLiveCameras->IsComplete() ? nullptr : CurrentLiveCameras;
}

void UCinemachineBlendManager::SetActiveBlend(UCinemachineBlend* Blend)
{
	SetRootBlend(Blend);
}

bool UCinemachineBlendManager::IsBlending() const
{
	return IsValid(GetActiveBlend());
}

bool UCinemachineBlendManager::IsLiveInBlend(ICinemachineCameraInterface* ICamera) const
{
	UObject* CameraObject = Cast<UObject>(ICamera);
	if (IsValid(CameraObject))
	{
		if (CurrentLiveCameras->CameraA == CameraObject)
		{
			return true;
		}
		else if (UCinemachineNestedBlendSource* NestedBlend = Cast<UCinemachineNestedBlendSource>(CurrentLiveCameras->CameraA))
		{
			if (IsValid(NestedBlend->Blend) && NestedBlend->Blend->Uses(ICamera))
			{
				return true;
			}
		}
	}
	return false;
}

bool UCinemachineBlendManager::IsLive(ICinemachineCameraInterface* ICamera) const
{
	return CurrentLiveCameras->Uses(ICamera);
}

void UCinemachineBlendManager::ComputeCurrentBlend()
{
	PreviousLiveCameras->CopyFrom(CurrentLiveCameras);
	ProcessOverrideFrames(CurrentLiveCameras, 0);
}

void UCinemachineBlendManager::RefreshCurrentCameraState(FVector Up, float DeltaTime)
{
	CurrentLiveCameras->UpdateCameraState(Up, DeltaTime);
}

ICinemachineCameraInterface* UCinemachineBlendManager::ProcessActiveCamera(ICinemachineMixerInterface* Mixer, FVector Up, float DeltaTime)
{
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld());
	if (!IsInitialized() && !IsValid(Subsystem))
	{
		return nullptr;
	}

	CameraCache.Empty();
	CollectLiveCameras(PreviousLiveCameras, CameraCache);

	TScriptInterface<ICinemachineMixerInterface> MixerInteface = GetCinemachineMixerInterface(Mixer);

	for (int32 i = 0; i < CameraCache.Num(); ++i)
	{
		if (!IsLive(Cast<ICinemachineCameraInterface>(CameraCache[i])))
		{
			Subsystem->OnCameraDeactivatedEvent.Broadcast(MixerInteface, CameraCache[i]);
		}
	}

	ICinemachineCameraInterface* IncomingCamera = GetActiveVirtualCamera();
	if (IsCinemachineCameraValid(IncomingCamera))
	{
		ICinemachineCameraInterface* OutgoingCamera = GetDeepCameraBFromBlend(PreviousLiveCameras);
		if (!IsCinemachineCameraValid(OutgoingCamera))
		{
			OutgoingCamera = nullptr;
		}

		if (IncomingCamera == OutgoingCamera)
		{
			if (!IsValid(CurrentLiveCameras->CameraA) && IsValid(PreviousLiveCameras->CameraA))
			{
				Subsystem->BlendFinishedEvent.Broadcast(MixerInteface, GetCinemachineCameraInterface(IncomingCamera));
			}
		}
		else
		{
			TScriptInterface<ICinemachineCameraInterface> EventOutgoing;
			EventOutgoing.SetObject(Cast<UObject>(OutgoingCamera));
			EventOutgoing.SetInterface(OutgoingCamera);

			bool bBlending = IsBlending();
			if (bBlending)
			{
				EObjectFlags Flags = RF_Public;
				Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
				if (HasAllFlags(RF_Transient))
				{
					Flags |= RF_Transient;
				}
				UCinemachineNestedBlendSource* NewBlend = NewObject<UCinemachineNestedBlendSource>(this, NAME_None, Flags);
				NewBlend->Blend = GetActiveBlend();
				NewBlend->UpdateCameraState(Up, DeltaTime);
				EventOutgoing.SetObject(NewBlend);
				EventOutgoing.SetInterface(NewBlend);
			}

			FCVActivationEventParams Params;
			Params.Origin.SetObject(Cast<UObject>(Mixer));
			Params.Origin.SetInterface(Mixer);
			Params.OutgoingCamera = EventOutgoing;
			Params.IncomingCamera.SetObject(Cast<UObject>(IncomingCamera));
			Params.IncomingCamera.SetInterface(IncomingCamera);
			Params.bCut = !bBlending;
			Params.WorldUp = Up;
			Params.DeltaTime = DeltaTime;

			IncomingCamera->OnCameraActivated(Params);
			Cast<ICinemachineCameraInterface>(Mixer)->OnCameraActivated(Params);
			Subsystem->CameraActivatedEvent.Broadcast(Params);

			IncomingCamera->UpdateCameraState(Up, DeltaTime);
		}
	}
	return IncomingCamera;
}

void UCinemachineBlendManager::OnInitailize()
{
	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (HasAllFlags(RF_Transient))
	{
		Flags |= RF_Transient;
	}

	CurrentLiveCameras = NewObject<UCinemachineBlend>(this, NAME_None, Flags);
	PreviousLiveCameras = NewObject<UCinemachineBlend>(this, NAME_None, Flags);
	CameraCache.Reserve(2);
}
