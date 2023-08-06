

#include "CinemachineCoreSubSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "CinemachineBrainComponent.h"
#include "CinemachineVirtualCameraBaseComponent.h"

//! FCinemachineGameplayMessageListenerHandle

void FCinemachineGameplayMessageListenerHandle::Unregister()
{
	if (UCinemachineCoreSubSystem* StrongSubsystem = Subsystem.Get())
	{
		Subsystem->UnregisterListener(*this);
		Subsystem.Reset();
		Channel = FGameplayTag();
		ID = 0;
	}
}

//! UCinemachineCoreSubSystem

void UCinemachineCoreSubSystem::Deinitialize()
{
	//! Camera
	ActiveCameras.Reset();
	AllCameras.Reset();
	// Brain
	ActiveBrains.Reset();
	// Event
	ListenerMap.Reset();

	Super::Deinitialize();
}

double UCinemachineCoreSubSystem::GetUnpausedTimeSeconds() const
{
	return UGameplayStatics::GetUnpausedTimeSeconds(this);
}

int32 UCinemachineCoreSubSystem::GetActiveCameraCount() const
{
	return ActiveCameras.Num();
}

UCinemachineVirtualCameraBaseComponent* UCinemachineCoreSubSystem::GetActiveCamera(int32 Index)
{
	if (!bActiveCamerasAreSorted && ActiveCameras.Num() > 0)
	{
		ActiveCameras.Sort([](const UCinemachineVirtualCameraBaseComponent& A, const UCinemachineVirtualCameraBaseComponent& B) -> bool
		{
			return A.Priority == B.Priority ? A.ActivationId > B.ActivationId : A.Priority > B.Priority;
		});
		bActiveCamerasAreSorted = true;
	}
	return ActiveCameras.IsValidIndex(Index) ? ActiveCameras[Index] : nullptr;
}

void UCinemachineCoreSubSystem::AddActiveCamera(UCinemachineVirtualCameraBaseComponent* VCamera)
{
	if (!IsValid(VCamera) && ActiveCameras.Contains(VCamera))
	{
		return;
	}
	VCamera->ActivationId = ActivationSequence++;
	ActiveCameras.Emplace(VCamera);
	bActiveCamerasAreSorted = false;
}

void UCinemachineCoreSubSystem::RemoveActiveCamera(UCinemachineVirtualCameraBaseComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return;
	}
	if (ActiveCameras.Contains(VCamera))
	{
		ActiveCameras.Remove(VCamera);
	}
}

void UCinemachineCoreSubSystem::CameraEnabled(UCinemachineVirtualCameraBaseComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return;
	}
	int32 ParentLevel = 0;
	UCinemachineVirtualCameraBaseComponent* ParentCamera = Cast<UCinemachineVirtualCameraBaseComponent>(VCamera->GetParentCamera().GetObject());
	for (; IsValid(ParentCamera); ParentCamera = Cast<UCinemachineVirtualCameraBaseComponent>(ParentCamera->GetParentCamera().GetObject()))
	{
		++ParentLevel;
	}
	while (AllCameras.Num() <= ParentLevel)
	{
		AllCameras.Emplace(FCVCameraArray());
	}
	AllCameras[ParentLevel].VirtualCameras.Emplace(VCamera);
}

void UCinemachineCoreSubSystem::CameraDisabled(UCinemachineVirtualCameraBaseComponent* VCamera)
{
	if (!IsValid(VCamera))
	{
		return;
	}
	for (int32 i = 0; i < AllCameras.Num(); ++i)
	{
		AllCameras[i].VirtualCameras.Remove(VCamera);
	}
}

void UCinemachineCoreSubSystem::UpdateAllActiveVirtualCameras(FVector WorldUp, float DeltaTime)
{
	if (!IsValid(RoundRobinVCameraLastFrame))
	{
		RoundRobinVCameraLastFrame = nullptr;
	}
	UCinemachineVirtualCameraBaseComponent* CurrentRoundRobin = RoundRobinVCameraLastFrame;

	for (int32 i = AllCameras.Num() - 1; i >= 0; --i)
	{
		FCVCameraArray SubCameras = AllCameras[i];
		for (int32 j = SubCameras.VirtualCameras.Num() - 1; j >= 0; --j)
		{
			UCinemachineVirtualCameraBaseComponent* VCamera = SubCameras.VirtualCameras[j];
			if (!IsValid(VCamera))
			{
				SubCameras.VirtualCameras.RemoveAt(j);
				continue;
			}
			if (VCamera == RoundRobinVCameraLastFrame)
			{
				CurrentRoundRobin = nullptr;
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

int32 UCinemachineCoreSubSystem::GetActiveBrainCount() const
{
	return ActiveBrains.Num();
}

UCinemachineBrainComponent* UCinemachineCoreSubSystem::GetActiveBrain(int32 Index) const
{
	return ActiveBrains.IsValidIndex(Index) ? ActiveBrains[Index] : nullptr;
}

void UCinemachineCoreSubSystem::AddActiveBrain(UCinemachineBrainComponent* Brain)
{
	if (!IsValid(Brain))
	{
		return;
	}
	ActiveBrains.Emplace(Brain);
}

void UCinemachineCoreSubSystem::RemoveActiveBrain(UCinemachineBrainComponent* Brain)
{
	if (!IsValid(Brain))
	{
		return;
	}
	ActiveBrains.Remove(Brain);
}

bool UCinemachineCoreSubSystem::IsLive(TScriptInterface<ICinemachineCameraInterface> ICamera)
{
	UObject* CameraObject = ICamera.GetObject();
	if (IsValid(CameraObject))
	{
		const int32 BrainCount = GetActiveBrainCount();
		TSet<int32> RemoveIndex;
		RemoveIndex.Reserve(BrainCount);
		for (int32 i = 0; i < BrainCount; ++i)
		{
			UCinemachineBrainComponent* Brain = ActiveBrains[i];
			if (IsValid(Brain))
			{
				if (Brain->IsLiveChild(Cast<ICinemachineCameraInterface>(CameraObject)))
				{
					return true;
				}
			}
			else
			{
				RemoveIndex.Add(i);
			}
		}

		for (int32 Index : RemoveIndex)
		{
			ActiveBrains.RemoveAt(Index);
		}
	}
	return false;
}

bool UCinemachineCoreSubSystem::IsLiveInBlend(TScriptInterface<ICinemachineCameraInterface> ICamera)
{
	UObject* CameraObject = ICamera.GetObject();
	if (IsValid(CameraObject))
	{
		const int32 BrainCount = GetActiveBrainCount();
		TSet<int32> RemoveIndex;
		RemoveIndex.Reserve(BrainCount);
		for (int32 i = 0; i < BrainCount; ++i)
		{
			UCinemachineBrainComponent* Brain = ActiveBrains[i];
			if (IsValid(Brain))
			{
				if (Brain->IsLiveInBlend(Cast<ICinemachineCameraInterface>(CameraObject)))
				{
					return true;
				}
			}
			else
			{
				RemoveIndex.Add(i);
			}
		}

		for (int32 Index : RemoveIndex)
		{
			ActiveBrains.RemoveAt(Index);
		}
	}
	return false;
}

void UCinemachineCoreSubSystem::ResetCameraState()
{
	TSet<int32> RemoveIndex;
	const int32 CameraCount = GetActiveCameraCount();
	const int32 BrainCount = GetActiveBrainCount();
	RemoveIndex.Reserve(FMath::Max(CameraCount, BrainCount));

	for (int32 i = 0; i < CameraCount; ++i)
	{
		UCinemachineVirtualCameraBaseComponent* VCamera = GetActiveCamera(i);
		if (IsValid(VCamera))
		{
			VCamera->SetPreviousStateIsValid(false);
		}
		else
		{
			RemoveIndex.Add(i);
		}
	}
	for (int32 Index : RemoveIndex)
	{
		ActiveCameras.RemoveAt(Index);
	}

	RemoveIndex.Empty();

	for (int32 i = 0; i < BrainCount; ++i)
	{
		UCinemachineBrainComponent* Brain = GetActiveBrain(i);
		if (IsValid(Brain))
		{
			Brain->ResetState();
		}
		else
		{
			RemoveIndex.Add(i);
		}
	}
	for (int32 Index : RemoveIndex)
	{
		ActiveBrains.RemoveAt(Index);
	}
}

UCinemachineBrainComponent* UCinemachineCoreSubSystem::FindPotentialTargetBrain(UCinemachineVirtualCameraBaseComponent* VCamera)
{
	if (IsValid(VCamera))
	{
		const int32 BrainCount = GetActiveBrainCount();
		for (int32 i = 0; i < BrainCount; ++i)
		{
			UCinemachineBrainComponent* Brain = GetActiveBrain(i);
			if (IsValid(Brain) && IsValid(Brain->GetOutputCamera()) && Brain->IsLiveChild(VCamera))
			{
				return Brain;
			}
		}
	}
	return nullptr;
}

FCinemachineGameplayMessageListenerHandle UCinemachineCoreSubSystem::RegisterListener(FGameplayTag Channel, TFunction<void(FGameplayTag)>&& Callback, EGameplayMessageMatch MatchType)
{
	return RegisterListenerInternal(Channel, MoveTemp(Callback), MatchType);
}

void UCinemachineCoreSubSystem::UnregisterListener(FCinemachineGameplayMessageListenerHandle Handle)
{
	if (Handle.IsValid())
	{
		check(Handle.Subsystem == this);
		UnregisterListenerInternal(Handle.Channel, Handle.ID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCinemachineCoreSubSystem] Trying to unregister an invalid Handle."));
	}
}

void UCinemachineCoreSubSystem::BroadcastMessage(FGameplayTagContainer Channel)
{
	BroadcastMessageInternal(Channel);
}

FCinemachineGameplayMessageListenerHandle UCinemachineCoreSubSystem::RegisterListenerInternal(FGameplayTag Channel, TFunction<void(FGameplayTag)>&& Callback, EGameplayMessageMatch MatchType)
{
	FChannelListenerList& List = ListenerMap.FindOrAdd(Channel);

	FCinemachineGameplayMessageListenerData& Entry = List.Listeners.Emplace_GetRef();
	Entry.ReceivedCallback = MoveTemp(Callback);
	Entry.HandleID = ++List.HandleID;
	Entry.MatchType = MatchType;

	return FCinemachineGameplayMessageListenerHandle(this, Channel, Entry.HandleID);
}

void UCinemachineCoreSubSystem::UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID)
{
	if (FChannelListenerList* pList = ListenerMap.Find(Channel))
	{
		int32 MatchIndex = pList->Listeners.IndexOfByPredicate([ID = HandleID](const FCinemachineGameplayMessageListenerData& Other) { return Other.HandleID == ID; });
		if (MatchIndex != INDEX_NONE)
		{
			pList->Listeners.RemoveAtSwap(MatchIndex);
		}

		if (pList->Listeners.Num() == 0)
		{
			ListenerMap.Remove(Channel);
		}
	}
}

void UCinemachineCoreSubSystem::BroadcastMessageInternal(FGameplayTagContainer Channel)
{
	for (auto Iter = Channel.CreateConstIterator(); Iter; ++Iter)
	{
		bool bOnInitialTag = true;
		for (FGameplayTag Tag = *Iter; Tag.IsValid(); Tag = Tag.RequestDirectParent())
		{
			if (const FChannelListenerList* pList = ListenerMap.Find(Tag))
			{
				// Copy in case there are removals while handling callbacks
				TArray<FCinemachineGameplayMessageListenerData> ListenerArray(pList->Listeners);

				for (const FCinemachineGameplayMessageListenerData& Listener : ListenerArray)
				{
					if (bOnInitialTag || (Listener.MatchType == EGameplayMessageMatch::PartialMatch))
					{
						// The receiving type must be either a parent of the sending type or completely ambiguous (for internal use)
						Listener.ReceivedCallback(Tag);
					}
				}
			}
			bOnInitialTag = false;
		}
	}
}
