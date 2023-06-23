
#include "CinemachineCoreSubSystem.h"
#include "CinemachineBrainComponent.h"
#include "CinemachineVirtualCameraComponent.h"

UCinemachineCoreSubSystem::UCinemachineCoreSubSystem()
	: UWorldSubsystem()
	, bSmartUpdate(true)
	, RoundRobinVCameraLastFrame(nullptr)
	, bActiveCameraAreSorted(false)
	, ActivationSequence(0)
{
}

int32 UCinemachineCoreSubSystem::GetBrainCameraCount() const
{
	return ActiveBrains.Num();
}

UCinemachineBrainComponent* UCinemachineCoreSubSystem::GetBrainCamera(int32 Index) const
{
	return ActiveBrains.IsValidIndex(Index) ? ActiveBrains[Index] : nullptr;
}

void UCinemachineCoreSubSystem::AddActiveBrain(UCinemachineBrainComponent* BrainComponent)
{
	if (IsValid(BrainComponent))
	{
		RemoveActiveBrain(BrainComponent);
		ActiveBrains.Add(BrainComponent);
	}
}

void UCinemachineCoreSubSystem::RemoveActiveBrain(UCinemachineBrainComponent* BrainComponent)
{
	if (IsValid(BrainComponent))
	{
		ActiveBrains.Remove(BrainComponent);
	}
}

int32 UCinemachineCoreSubSystem::GetVirtualCameraCount() const
{
	return ActiveVirtualCameras.Num();
}

UCinemachineVirtualCameraComponent* UCinemachineCoreSubSystem::GetVirtualCamera(int32 Index)
{
	if (!bActiveCameraAreSorted && ActiveVirtualCameras.Num() > 0)
	{
		ActiveVirtualCameras.Sort([](const UCinemachineVirtualCameraComponent& A, const UCinemachineVirtualCameraComponent& B) -> bool
		{
			return A.GetPriority() == B.GetPriority() ? A.ActivationId > B.ActivationId : A.GetPriority() > B.GetPriority();
		});
		bActiveCameraAreSorted = true;
	}
	return ActiveVirtualCameras.IsValidIndex(Index) ? ActiveVirtualCameras[Index] : nullptr;
}

void UCinemachineCoreSubSystem::AddActiveCamera(UCinemachineVirtualCameraComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return;
	}
	checkf(!ActiveVirtualCameras.Contains(VCamera), TEXT("VirtualCameraComponent is already in ActiveVirtualCameras [%s]"), *VCamera->GetCameraName());
	VCamera->ActivationId = ActivationSequence++;
	ActiveVirtualCameras.Emplace(VCamera);
	bActiveCameraAreSorted = false;
}

void UCinemachineCoreSubSystem::RemoveActiveCamera(UCinemachineVirtualCameraComponent* VCamera)
{
	if (IsValid(VCamera))
	{
		ActiveVirtualCameras.Remove(VCamera);
	}
}

void UCinemachineCoreSubSystem::CameraEnabled(UCinemachineVirtualCameraComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return;
	}

	int32 ParentLevel = 0;
	for (UCinemachineVirtualCameraComponent* ParentCamera = VCamera->GetParentCamera(); IsValid(ParentCamera); ParentCamera = ParentCamera->GetParentCamera())
	{
		++ParentLevel;
	}
	while (AllVirtualCameras.Num() <= ParentLevel)
	{
		AllVirtualCameras.Emplace(TArray<UCinemachineVirtualCameraComponent*>());;
	}
	AllVirtualCameras[ParentLevel].Emplace(VCamera);
}

void UCinemachineCoreSubSystem::CameraDisabled(UCinemachineVirtualCameraComponent* VirtualCameraComponent)
{
	if (!IsValid(VirtualCameraComponent))
	{
		return;
	}

	for (int32 i = 0; i < AllVirtualCameras.Num(); ++i)
	{
		AllVirtualCameras[i].Remove(VirtualCameraComponent);
	}
	if (RoundRobinVCameraLastFrame == VirtualCameraComponent)
	{
		RoundRobinVCameraLastFrame = nullptr;
	}
}

void UCinemachineCoreSubSystem::UpdateAllActiveVirtualCameras(FVector WorldUp, float DeltaTime)
{
	UCinemachineVirtualCameraComponent* CurrentRoundRobin = RoundRobinVCameraLastFrame;

	for (int32 i = AllVirtualCameras.Num() - 1; i >= 0; --i)
	{
		TArray<UCinemachineVirtualCameraComponent*> SubCameras = AllVirtualCameras[i];
		for (int32 j = SubCameras.Num() - 1; j >= 0; --j)
		{
			UCinemachineVirtualCameraComponent* VCamera = SubCameras[j];
			if (VCamera == RoundRobinVCameraLastFrame)
			{
				RoundRobinVCameraLastFrame = nullptr;
			}
			if (!IsValid(VCamera))
			{
				SubCameras.RemoveAt(j);
				continue;
			}
			if (VCamera->StandbyUpdateMode == ECineamchineStandbyUpdateMode::Always || IsLive(VCamera))
			{
				UpdateVirtualCamera(VCamera, WorldUp, DeltaTime);
			}
			else if (!IsValid(CurrentRoundRobin) && VCamera != RoundRobinVCameraLastFrame && VCamera->StandbyUpdateMode != ECineamchineStandbyUpdateMode::Nerver && VCamera->IsActive())
			{
				UpdateVirtualCamera(VCamera, WorldUp, DeltaTime);
				CurrentRoundRobin = VCamera;
			}
		}
	}

	if (CurrentRoundRobin == RoundRobinVCameraLastFrame)
	{
		CurrentRoundRobin = nullptr;
	}
	RoundRobinVCameraLastFrame = CurrentRoundRobin;
}

void UCinemachineCoreSubSystem::UpdateVirtualCamera(UCinemachineVirtualCameraComponent* VCamera, FVector WorldUp, float DeltaTime)
{
	if (!IsValid(VCamera))
	{
		return;
	}

	if (bSmartUpdate)
	{
		AActor* UpdateTarget = GetUpdateTarget(VCamera);
		if (!IsValid(UpdateTarget))
		{
			return;
		}
	}

	VCamera->InternalUpdateCameraState(WorldUp, DeltaTime);
}

AActor* UCinemachineCoreSubSystem::GetUpdateTarget(UCinemachineVirtualCameraComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return nullptr;
	}
	AActor* Target = VCamera->GetLookAt();
	if (IsValid(Target))
	{
		return Target;
	}
	Target = VCamera->GetFollow();
	if (IsValid(Target))
	{
		return Target;
	}
	return VCamera->GetOwner();
}

bool UCinemachineCoreSubSystem::IsLive(UObject* ICamera)
{
	if (!IsValid(ICamera))
	{
		return false;
	}
	ICinemachineCameraInterface* CameraInterface = Cast<ICinemachineCameraInterface>(ICamera);
	if (nullptr == CameraInterface)
	{
		return false;
	}
	for (int32 i = 0; i < GetBrainCameraCount(); ++i)
	{
		UCinemachineBrainComponent* BrainComponent = GetBrainCamera(i);
		if (IsValid(BrainComponent) && BrainComponent->IsLive(ICamera))
		{
			return true;
		}
	}
	return false;
}

bool UCinemachineCoreSubSystem::IsLiveBlend(UObject* ICamera)
{
	if (!IsValid(ICamera))
	{
		return false;
	}
	ICinemachineCameraInterface* CameraInterface = Cast<ICinemachineCameraInterface>(ICamera);
	if (nullptr == CameraInterface)
	{
		return false;
	}
	for (int32 i = 0; i < GetBrainCameraCount(); ++i)
	{
		UCinemachineBrainComponent* BrainComponent = GetBrainCamera(i);
		if (IsValid(BrainComponent) && BrainComponent->IsLiveBlend(ICamera))
		{
			return true;
		}
	}
	return false;
}

void UCinemachineCoreSubSystem::GenerateCameraActivationEvent(UCinemachineVirtualCameraComponent* VCamera, UCinemachineVirtualCameraComponent* VCameraFrom)
{
	if (!IsValid(VCamera))
	{
		return;
	}
	for (int32 i = 0; i < GetBrainCameraCount(); ++i)
	{
		UCinemachineBrainComponent* BrainComponent = GetBrainCamera(i);
		if (IsValid(BrainComponent) && BrainComponent->IsLive(VCamera))
		{
			BrainComponent->CameraActivatedEvent.Broadcast(VCamera, VCameraFrom);
		}
	}
}

void UCinemachineCoreSubSystem::GenerateCameraCutEvent(UCinemachineVirtualCameraComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return;
	}
	for (int32 i = 0; i < GetBrainCameraCount(); ++i)
	{
		UCinemachineBrainComponent* BrainComponent = GetBrainCamera(i);
		if (IsValid(BrainComponent) && BrainComponent->IsLive(VCamera))
		{
			BrainComponent->CameraCutEvent.Broadcast(BrainComponent);
		}
	}
}

UCinemachineBrainComponent* UCinemachineCoreSubSystem::FindPotentialTargetBrain(UCinemachineVirtualCameraComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return nullptr;
	}

	const int32 BrainCount = GetBrainCameraCount();
	for (int32 i = 0; i < BrainCount; ++i)
	{
		UCinemachineBrainComponent* BrainComponent = GetBrainCamera(i);
		if (IsValid(BrainComponent) && IsValid(BrainComponent->GetOutputCamera()) && BrainComponent->IsLive(VCamera))
		{
			return BrainComponent;
		}
	}

	for (int32 i = 0; i < BrainCount; ++i)
	{
		UCinemachineBrainComponent* BrainComponent = GetBrainCamera(i);
		if (IsValid(BrainComponent) && IsValid(BrainComponent->GetOutputCamera()))
		{
			return BrainComponent;
		}
	}

	return nullptr;
}

void UCinemachineCoreSubSystem::OnTargetObjectWarped(AActor* Target, FVector LocationDelta)
{
	for (int32 i = 0; i < GetVirtualCameraCount(); ++i)
	{
		UCinemachineVirtualCameraComponent* VCamera = GetVirtualCamera(i);
		if (IsValid(VCamera))
		{
			VCamera->OnTargetObjectWarped(Target, LocationDelta);
		}
	}
}

