

#include "CinemachineBrainComponent.h"
#include "Camera/CameraComponent.h"
#include "CinemachineVirtualCameraBaseComponent.h"
#include "CinemachineCoreSubSystem.h"
#include "CinemachineBlendManager.h"

//! UCinemachineBrainComponent

UCinemachineBrainComponent::UCinemachineBrainComponent()
	: bBrainEnabled(true)
{
	bCacheBrainEnabled = bBrainEnabled;

	PrimaryComponentTick.bCanEverTick = true;
}

void UCinemachineBrainComponent::BeginPlay()
{
	Super::BeginPlay();

	Init();

	if (true == bBrainEnabled)
	{
		bCacheBrainEnabled = !bBrainEnabled;
		SetEnable(true);
	}
	else
	{
		bCacheBrainEnabled = bBrainEnabled;
	}
}

void UCinemachineBrainComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SetEnable(false);
	Super::EndPlay(EndPlayReason);
}

void UCinemachineBrainComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsEnable())
	{
		return;
	}

	if (TickType == ELevelTick::LEVELTICK_All)
	{
		ManualUpdate(DeltaTime);
	}
}

int32 UCinemachineBrainComponent::SetCameraOverride(int32 OverrideId, int32 Priority, ICinemachineCameraInterface* CameraA, ICinemachineCameraInterface* CameraB, float WeightB, float DeltaTime)
{
	if (IsValid(BlendManager))
	{
		return BlendManager->SetCameraOverride(OverrideId, Priority, CameraA, CameraB, WeightB, DeltaTime);
	}
	return INDEX_NONE;
}

void UCinemachineBrainComponent::ReleaseCameraOverride(int32 OverrideId)
{
	if (IsValid(BlendManager))
	{
		BlendManager->ReleaseCameraOverride(OverrideId);
	}
}

FVector UCinemachineBrainComponent::DefaultWorldUp() const
{
	return IsValid(WorldUpOverride) ? WorldUpOverride->GetUpVector() : FVector::UpVector;
}

bool UCinemachineBrainComponent::IsLiveChild(ICinemachineCameraInterface* ICamera, bool bDominantChildOnly) const
{
	if (!IsValid(BlendManager) || !IsCinemachineCameraValid(ICamera))
	{
		return false;
	}

	if (BlendManager->IsLive(ICamera))
	{
		return true;
	}

	TScriptInterface<ICinemachineMixerInterface> Parent = ICamera->GetParentCamera();
	UObject* ParentObject = Parent.GetObject();
	if (Parent && IsValid(ParentObject) && Parent->IsLiveChild(ICamera, bDominantChildOnly))
	{
		return IsLiveChild(Cast<ICinemachineCameraInterface>(ParentObject), bDominantChildOnly);
	}

	return false;
}

void UCinemachineBrainComponent::Init()
{
	if (bIsInitialized)
	{
		return;
	}
	bIsInitialized = true;

	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (HasAllFlags(RF_Transient))
	{
		Flags |= RF_Transient;
	}
	BlendManager = NewObject<UCinemachineBlendManager>(this, NAME_None, Flags);
	BlendManager->Init();

	if (IsValid(TargetOverride))
	{
		OutputCamera = TargetOverride;
	}
	else
	{
		AActor* Owner = GetOwner();
		if (Owner->bFindCameraComponentWhenViewTarget)
		{
			for (UActorComponent* Component : Owner->GetComponents())
			{
				UCameraComponent* CameraComponent = Cast<UCameraComponent>(Component);
				if (CameraComponent)
				{
					if (CameraComponent->IsActive())
					{
						OutputCamera = CameraComponent;
						break;
					}
				}
			}
		}
	}
}

void UCinemachineBrainComponent::SetEnable(bool bEnable)
{
	if (bCacheBrainEnabled == bEnable)
	{
		return;
	}
	bBrainEnabled = bEnable;

	if (bEnable)
	{
		Init();
		SetComponentTickEnabled(true);
		OnEnable();
	}
	else
	{
		OnDisable();
		SetComponentTickEnabled(false);
	}
}

TScriptInterface<ICinemachineCameraInterface> UCinemachineBrainComponent::GetActiveVirtualCamera() const
{
	TScriptInterface<ICinemachineCameraInterface> ActiveCamera = nullptr;
	if (IsValid(BlendManager))
	{
		if (ICinemachineCameraInterface* ICamera = BlendManager->GetActiveVirtualCamera())
		{
			ActiveCamera.SetObject(Cast<UObject>(ICamera));
			ActiveCamera.SetInterface(ICamera);
		}
	}
	return ActiveCamera;
}

void UCinemachineBrainComponent::ResetState()
{
	if (IsValid(BlendManager))
	{
		BlendManager->ResetRootFrame();
	}
}

bool UCinemachineBrainComponent::IsBlending() const
{
	return IsValid(BlendManager) ? BlendManager->IsBlending() : false;
}

UCinemachineBlend* UCinemachineBrainComponent::GetActiveBlend() const
{
	return IsValid(BlendManager) ? BlendManager->GetActiveBlend() : nullptr;
}

void UCinemachineBrainComponent::SetActiveBlend(UCinemachineBlend* Value)
{
	if (IsValid(BlendManager))
	{
		BlendManager->SetActiveBlend(Value);
	}
}

bool UCinemachineBrainComponent::IsLiveInBlend(ICinemachineCameraInterface* ICamera)
{
	if (!IsValid(BlendManager) || !IsCinemachineCameraValid(ICamera))
	{
		return false;
	}

	if (BlendManager->IsLiveInBlend(ICamera))
	{
		return true;
	}

	TScriptInterface<ICinemachineMixerInterface> Parent = ICamera->GetParentCamera();
	UObject* ParentObject = Parent.GetObject();
	if (Parent && IsValid(ParentObject) && Parent->IsLiveChild(ICamera, false))
	{
		return IsLiveInBlend(Cast<ICinemachineCameraInterface>(ParentObject));
	}
	return false;
}

void UCinemachineBrainComponent::ManualUpdate(float DeltaTime)
{
	if (!IsValid(BlendManager))
	{
		return;
	}
	
	BlendManager->UpdateRootFrame(this, TopCameraFromPriorityQueue(), DefaultWorldUp(), DeltaTime);
	BlendManager->ComputeCurrentBlend();
	UpdateVirtualCameras(DeltaTime);
	ProcessActiveCamera(DeltaTime);
}

void UCinemachineBrainComponent::OnEnable()
{
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld());
	if (IsValid(Subsystem))
	{
		Subsystem->AddActiveBrain(this);
	}

	if (IsValid(GetOutputCamera()))
	{
		CachedOutputCameraLocation = OutputCamera->GetRelativeLocation();
		CachedOutputCameraRotation = OutputCamera->GetRelativeRotation();
		OutputCamera->SetAbsolute(true, true, true);
	}

	if (IsValid(BlendManager))
	{
		BlendManager->SetEnable(true);
		BlendManager->LookupBlend = DefaultBlend;
	}
}

void UCinemachineBrainComponent::OnDisable()
{
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld());
	if (IsValid(Subsystem))
	{
		Subsystem->RemoveActiveBrain(this);
	}

	if (IsValid(GetOutputCamera()))
	{
		OutputCamera->SetAbsolute(false, false, false);
		OutputCamera->SetRelativeLocationAndRotation(CachedOutputCameraLocation, CachedOutputCameraRotation);
	}

	if (IsValid(BlendManager))
	{
		BlendManager->SetEnable(false);
	}
}

void UCinemachineBrainComponent::UpdateVirtualCameras(float DeltaTime)
{
	if (!IsValid(BlendManager))
	{
		return;
	}
	
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld());
	if (!IsValid(Subsystem))
	{
		return;
	}

	const FVector WorldUp = DefaultWorldUp();
	Subsystem->UpdateAllActiveVirtualCameras(WorldUp, DeltaTime);
	BlendManager->RefreshCurrentCameraState(WorldUp, DeltaTime);
}

UCinemachineVirtualCameraBaseComponent* UCinemachineBrainComponent::TopCameraFromPriorityQueue()
{
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld());
	if (!IsValid(Subsystem))
	{
		return nullptr;
	}

	const int32 CameraCount = Subsystem->GetActiveCameraCount();
	for (int32 i = 0; i < CameraCount; ++i)
	{
		UCinemachineVirtualCameraBaseComponent* VCamera = Subsystem->GetActiveCamera(i);
		if (IsValid(VCamera))
		{
			return VCamera;
		}
	}

	return nullptr;
}

void UCinemachineBrainComponent::ProcessActiveCamera(float DeltaTime)
{
	if (!IsValid(BlendManager))
	{
		return;
	}

	if (IsValid(BlendManager->CurrentLiveCameras) && BlendManager->ProcessActiveCamera(this, DefaultWorldUp(), DeltaTime))
	{
		PushStateToCamera(BlendManager->CurrentLiveCameras->GetState());
	}
	else
	{
		FCinemachineCameraState NewState;
		if (UCameraComponent* Camera = GetOutputCamera())
		{
			NewState.RawLocation = Camera->GetComponentLocation();
			NewState.RawOrientation = Camera->GetComponentRotation();
			NewState.Lens = FCinemachineLensSettings::FromCamera(Camera);
			NewState.BlendHint |= (static_cast<int32>(ECVBlendHintValue::NoTransform) | static_cast<int32>(ECVBlendHintValue::NoLens));
			PushStateToCamera(NewState);
		}
	}
}

void UCinemachineBrainComponent::PushStateToCamera(FCinemachineCameraState State)
{
	CurrentCameraState = State;
	UCameraComponent* Camera = GetOutputCamera();
	if (!IsValid(Camera))
	{
		return;
	}

	bool bLocation = (State.BlendHint & static_cast<int32>(ECVBlendHintValue::NoLocation)) == 0;
	bool bOrientation = (State.BlendHint & static_cast<int32>(ECVBlendHintValue::NoOrientation)) == 0;
	if (bLocation && bOrientation)
	{
		Camera->SetWorldLocationAndRotation(CurrentCameraState.GetFinalLocation(), CurrentCameraState.GetFinalOrientation());
	}

	if ((State.BlendHint & static_cast<int32>(ECVBlendHintValue::NoLocation)) == 0)
	{

	}
	if ((State.BlendHint & static_cast<int32>(ECVBlendHintValue::NoLens)) == 0)
	{
		if (IsValid(Camera))
		{
			Camera->FieldOfView = CurrentCameraState.Lens.FieldOfView;
			Camera->AspectRatio = CurrentCameraState.Lens.AspectRatio;
		}
	}

	if (UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld()))
	{
		Subsystem->OnCameraUpdatedEvent.Broadcast(this);
	}
}
