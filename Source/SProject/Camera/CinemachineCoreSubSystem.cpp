

#include "CinemachineCoreSubSystem.h"
#include "CinemachineBrainComponent.h"
#include "Camera/CameraComponent.h"
#include "CinemachineVirtualCameraBaseComponent.h"

UCinemachineCoreSubSystem::UCinemachineCoreSubSystem()
	: UWorldSubsystem()
	, RoundRobinVCameraLastFrame(nullptr)
	, bActiveCameraAreSorted(false)
	, ActivationSequence(0)
{
}

int32 UCinemachineCoreSubSystem::GetBrainCameraCount() const
{
	return ActiveBrains.Num();
}

UCinemachineBrainComponent* UCinemachineCoreSubSystem::GetBrainCamera(int32 Index)
{
	if (!ActiveBrains.IsValidIndex(Index))
	{
		return nullptr;
	}
	UCinemachineBrainComponent* Result = ActiveBrains[Index];

	if (!IsValid(Result))
	{
		ActiveBrains.RemoveAt(Index);
		return nullptr;
	}

	return Result;
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

UCinemachineVirtualCameraBaseComponent* UCinemachineCoreSubSystem::GetVirtualCamera(int32 Index)
{
	if (!bActiveCameraAreSorted && ActiveVirtualCameras.Num() > 0)
	{
		ActiveVirtualCameras.Sort([](const UCinemachineVirtualCameraBaseComponent& A, const UCinemachineVirtualCameraBaseComponent& B) -> bool
		{
			return A.GetPriority() == B.GetPriority() ? A.ActivationId > B.ActivationId : A.GetPriority() > B.GetPriority();
		});
		bActiveCameraAreSorted = true;
	}
	return ActiveVirtualCameras.IsValidIndex(Index) ? ActiveVirtualCameras[Index] : nullptr;
}

void UCinemachineCoreSubSystem::AddActiveCamera(UCinemachineVirtualCameraBaseComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return;
	}
	checkf(!ActiveVirtualCameras.Contains(VCamera), TEXT("{%s}는 이미 등록 된 CV카메라 입니다."), *VCamera->GetCameraName());
	VCamera->ActivationId = ActivationSequence++;
	ActiveVirtualCameras.Emplace(VCamera);
	bActiveCameraAreSorted = false;
}

void UCinemachineCoreSubSystem::RemoveActiveCamera(UCinemachineVirtualCameraBaseComponent* VCamera)
{
	if (IsValid(VCamera))
	{
		ActiveVirtualCameras.Remove(VCamera);
	}
}

void UCinemachineCoreSubSystem::CameraEnabled(UCinemachineVirtualCameraBaseComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return;
	}

	int32 ParentLevel = 0;
	UCinemachineVirtualCameraBaseComponent* ParentCamera = Cast<UCinemachineVirtualCameraBaseComponent>(VCamera->GetParentCamera());
	for (; IsValid(ParentCamera); ParentCamera = Cast<UCinemachineVirtualCameraBaseComponent>(ParentCamera->GetParentCamera()))
	{
		++ParentLevel;
	}
	while (AllVirtualCameras.Num() <= ParentLevel)
	{
		AllVirtualCameras.Emplace(TArray<UCinemachineVirtualCameraBaseComponent*>());;
	}
	AllVirtualCameras[ParentLevel].Emplace(VCamera);
}

void UCinemachineCoreSubSystem::CameraDisabled(UCinemachineVirtualCameraBaseComponent* VirtualCameraComponent)
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
	UCinemachineVirtualCameraBaseComponent* CurrentRoundRobin = RoundRobinVCameraLastFrame;

	for (int32 i = AllVirtualCameras.Num() - 1; i >= 0; --i)
	{
		TArray<UCinemachineVirtualCameraBaseComponent*> SubCameras = AllVirtualCameras[i];
		for (int32 j = SubCameras.Num() - 1; j >= 0; --j)
		{
			UCinemachineVirtualCameraBaseComponent* VCamera = SubCameras[j];
			if (VCamera == RoundRobinVCameraLastFrame)
			{
				CurrentRoundRobin = nullptr;
			}
			if (!IsValid(VCamera))
			{
				SubCameras.RemoveAt(j);
				continue;
			}
			if (VCamera->StandbyUpdateMode == ECVStandbyUpdateMode::Always || IsLive(VCamera))
			{
				UpdateVirtualCamera(VCamera, WorldUp, DeltaTime);
			}
			else if (!IsValid(CurrentRoundRobin) && VCamera != RoundRobinVCameraLastFrame && VCamera->StandbyUpdateMode != ECVStandbyUpdateMode::Nerver && VCamera->IsEnable())
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

void UCinemachineCoreSubSystem::UpdateVirtualCamera(UCinemachineVirtualCameraBaseComponent* VCamera, FVector WorldUp, float DeltaTime)
{
	if (!IsValid(VCamera))
	{
		return;
	}
	VCamera->InternalUpdateCameraState(WorldUp, DeltaTime);
}

USceneComponent* UCinemachineCoreSubSystem::GetUpdateTarget(UCinemachineVirtualCameraBaseComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return nullptr;
	}
	USceneComponent* Target = VCamera->GetLookAt();
	if (IsValid(Target))
	{
		return Target;
	}
	Target = VCamera->GetFollow();
	if (IsValid(Target))
	{
		return Target;
	}
	return VCamera;
}

bool UCinemachineCoreSubSystem::IsLive(ICinemachineCameraInterface* ICamera)
{
	if (nullptr == ICamera)
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

bool UCinemachineCoreSubSystem::IsLiveInBlend(ICinemachineCameraInterface* ICamera)
{
	if (nullptr == ICamera)
	{
		return false;
	}
	for (int32 i = 0; i < GetBrainCameraCount(); ++i)
	{
		UCinemachineBrainComponent* BrainComponent = GetBrainCamera(i);
		if (IsValid(BrainComponent) && BrainComponent->IsLiveInBlend(ICamera))
		{
			return true;
		}
	}
	return false;
}

void UCinemachineCoreSubSystem::GenerateCameraActivationEvent(UCinemachineVirtualCameraBaseComponent* VCamera, UCinemachineVirtualCameraBaseComponent* VCameraFrom)
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

void UCinemachineCoreSubSystem::GenerateCameraCutEvent(UCinemachineVirtualCameraBaseComponent* VCamera)
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
			CameraCutEvent.Broadcast(BrainComponent);
		}
	}
}

UCinemachineBrainComponent* UCinemachineCoreSubSystem::FindPotentialTargetBrain(UCinemachineVirtualCameraBaseComponent* VCamera)
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
