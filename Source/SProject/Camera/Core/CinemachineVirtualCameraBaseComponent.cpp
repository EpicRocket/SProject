

#include "CinemachineVirtualCameraBaseComponent.h"
#include "CinemachineBlend.h"
#include "CinemachineExtension.h"
#include "CinemachineCoreSubSystem.h"
#include "CinemachineTargetGroupComponent.h"
#include "CinemachineBaseStage.h"
#include "CinemachineBrainComponent.h"
#include "Camera/Interface/CinemachineMixerInterface.h"
#include "Camera/VirtualCamera/CinemachineCameraManager.h"
#include "Shared/Damper.h"
#include "Shared/VectorExtension.h"

UCinemachineVirtualCameraBaseComponent::UCinemachineVirtualCameraBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCinemachineVirtualCameraBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TickType == ELevelTick::LEVELTICK_All)
	{
		if (!IsEnable())
		{
			return;
		}

		if (Priority != QueuePriority)
		{
			UpdateVcamPoolStatus();
		}
	}
}

TScriptInterface<ICinemachineMixerInterface> UCinemachineVirtualCameraBaseComponent::GetParentCamera()
{
	if (!bChildStatusUpdated)
	{
		UpdateStatusAsChild();
	}
	if (IsValid(ParentCamera) && ParentCamera->IsCameraValid())
	{
		ICinemachineMixerInterface* Mixer = Cast<ICinemachineMixerInterface>(ParentCamera);
		if (Mixer == nullptr)
		{
			return nullptr;
		}
		TScriptInterface<ICinemachineMixerInterface> MixerInterface;
		MixerInterface.SetObject(ParentCamera);
		MixerInterface.SetInterface(Mixer);
		return MixerInterface;
	}
	return nullptr;
}

void UCinemachineVirtualCameraBaseComponent::UpdateCameraState(FVector WorldUp, float DeltaTime)
{
	if (UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld()))
	{
		Subsystem->UpdateVirtualCamera(this, WorldUp, DeltaTime);
	}
}

void UCinemachineVirtualCameraBaseComponent::Init()
{
	if (true == bIsInitialized)
	{
		return;
	}
	bIsInitialized = true;
	OnInitailize();
}

void UCinemachineVirtualCameraBaseComponent::SetEnable(bool bEnable)
{
	if (bCacheCameraEnable == bEnable)
	{
		return;
	}
	bCameraEnable = bEnable;

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

	bCacheCameraEnable = bCameraEnable;
}

void UCinemachineVirtualCameraBaseComponent::OnCameraActivated(FCVActivationEventParams Params)
{
	UObject* IncomingCameraObject = Params.IncomingCamera.GetObject();
	if (IncomingCameraObject == this)
	{
		OnTransitionFromCamera(Params.OutgoingCamera, Params.WorldUp, Params.DeltaTime);
	}
}

void UCinemachineVirtualCameraBaseComponent::OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime)
{
	if (!IsEnable())
	{
		SetPreviousStateIsValid(false);
	}
}

float UCinemachineVirtualCameraBaseComponent::GetMaxDampTime()
{
	float MaxDampTime = 0.0F;
	for (UCinemachineExtension* Extension : Extensions)
	{
		MaxDampTime = FMath::Max(MaxDampTime, Extension->GetMaxDampTime());
	}
	return MaxDampTime;
}

float UCinemachineVirtualCameraBaseComponent::DetachedFollowTargetDamp(float Initial, float DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FMath::Max(1.0F, DampTime), DampTime, FollowTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, FollowTargetAttachment);
	return UDamper::Damp(Initial, DampTime, DeltaTime);
}

FVector UCinemachineVirtualCameraBaseComponent::DetachedFollowTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FVector::Max(FVector::OneVector, DampTime), DampTime, FollowTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, FollowTargetAttachment);
	return UDamper::Damp(Initial, DampTime, DeltaTime);
}

FVector UCinemachineVirtualCameraBaseComponent::DetachedFollowTargetDamp(FVector Initial, float DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FMath::Max(1.0F, DampTime), DampTime, FollowTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, FollowTargetAttachment);
	return UDamper::Damp(Initial, DampTime, DeltaTime);
}

float UCinemachineVirtualCameraBaseComponent::DetachedLookAtTargetDamp(float Initial, float DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FMath::Max(1.0F, DampTime), DampTime, LookAtTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, LookAtTargetAttachment);
	return UDamper::Damp(Initial, DampTime, DeltaTime);
}

FVector UCinemachineVirtualCameraBaseComponent::DetachedLookAtTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FVector::Max(FVector::OneVector, DampTime), DampTime, LookAtTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, LookAtTargetAttachment);
	return UDamper::Damp(Initial, DampTime, DeltaTime);
}

FVector UCinemachineVirtualCameraBaseComponent::DetachedLookAtTargetDamp(FVector Initial, float DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FMath::Max(1.0F, DampTime), DampTime, LookAtTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, LookAtTargetAttachment);
	return UDamper::Damp(Initial, DampTime, DeltaTime);
}

USceneComponent* UCinemachineVirtualCameraBaseComponent::ResolveFollow(USceneComponent* LocalFollow) const
{
	return !IsValid(LocalFollow) && IsValid(ParentCamera) ? ParentCamera->GetFollow() : LocalFollow;
}

USceneComponent* UCinemachineVirtualCameraBaseComponent::ResolveLookAt(USceneComponent* LocalLookAt) const
{
	return !IsValid(LocalLookAt) && IsValid(ParentCamera) ? ParentCamera->GetLookAt() : LocalLookAt;
}

void UCinemachineVirtualCameraBaseComponent::MoveToTopOfPrioritySubQueue()
{
	UpdateVcamPoolStatus();
}

void UCinemachineVirtualCameraBaseComponent::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	for (UCinemachineExtension* Extension : Extensions)
	{
		Extension->ForceCameraLocation(Location, Rotation);
	}
}

void UCinemachineVirtualCameraBaseComponent::AddExtension(UCinemachineExtension* Extension)
{
	if (IsValid(Extension) && !Extensions.Contains(Extension))
	{
		Extensions.Emplace(Extension);
	}
}

void UCinemachineVirtualCameraBaseComponent::RemoveExtension(UCinemachineExtension* Extension)
{
	if (IsValid(Extension) && Extensions.Contains(Extension))
	{
		Extensions.Remove(Extension);
	}
}

UCinemachineTargetGroupComponent* UCinemachineVirtualCameraBaseComponent::GetFollowTargetGroup()
{
	return CachedFollowTargetGroup;
}

UCinemachineVirtualCameraBaseComponent* UCinemachineVirtualCameraBaseComponent::GetFollowTargetAsVCamera()
{
	return CachedFollowTargetVCamera;
}

UCinemachineTargetGroupComponent* UCinemachineVirtualCameraBaseComponent::GetLookAtTargetGroup()
{
	return CachedLookAtTargetGroup;
}

UCinemachineVirtualCameraBaseComponent* UCinemachineVirtualCameraBaseComponent::GetLookAtTargetAsVCamera()
{
	return CachedLookAtTargetVCamera;
}

bool UCinemachineVirtualCameraBaseComponent::IsLive()
{
	if(UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld()))
	{
		return Subsystem->IsLive(this);
	}
	return false;
}

bool UCinemachineVirtualCameraBaseComponent::IsParticipatingInBlend()
{
	if (IsLive())
	{
		UCinemachineCameraManager* Manager = Cast<UCinemachineCameraManager>(GetParentCamera().GetObject());
		if (IsValid(Manager))
		{
			UCinemachineBlend* ActiveBlend = Manager->GetActiveBlend();
			return (IsValid(ActiveBlend) && ActiveBlend->Uses(this)) || Manager->IsParticipatingInBlend();
		}

		if(UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld()))
		{
			UCinemachineBrainComponent* Brain = Subsystem->FindPotentialTargetBrain(this);
			if (IsValid(Brain))
			{
				UCinemachineBlend* ActiveBlend = Brain->GetActiveBlend();
				return IsValid(ActiveBlend) && ActiveBlend->Uses(this);
			}
		}

	}
	return false;
}

void UCinemachineVirtualCameraBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	Init();

	if (true == bCameraEnable)
	{
		bCacheCameraEnable = !bCameraEnable;
		SetEnable(true);
	}
	else
	{
		bCacheCameraEnable = bCameraEnable;
	}
}

void UCinemachineVirtualCameraBaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SetEnable(false);
}

void UCinemachineVirtualCameraBaseComponent::OnEnable()
{
	UCinemachineCoreSubSystem* SubSystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld());
	if (!IsValid(SubSystem))
	{
		return;
	}

	UpdateStatusAsChild();
	UpdateVcamPoolStatus();
	if (!SubSystem->IsLive(this))
	{
		bPreviousStateIsValid = false;
	}
	SubSystem->CameraEnabled(this);
	InvalidateCachedTargets();
}

void UCinemachineVirtualCameraBaseComponent::OnDisable()
{
	if (UCinemachineCoreSubSystem* SubSystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld()))
	{
		UpdateVcamPoolStatus();
		SubSystem->CameraDisabled(this);
	}
}

void UCinemachineVirtualCameraBaseComponent::InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStageType Stage, FCinemachineCameraState& State, float DeltaTime)
{
	for (int32 i = 0; i < Extensions.Num(); ++i)
	{
		UCinemachineExtension* Extension = Extensions[i];
		if (!IsValid(Extension))
		{
			Extensions.RemoveAt(i);
			--i;
		}
		else
		{
			Extension->InvokePostPipelineStageCallback(VCamera, Stage, State, DeltaTime);
		}
	}

	UCinemachineVirtualCameraBaseComponent* Parent = Cast<UCinemachineVirtualCameraBaseComponent>(GetParentCamera().GetObject());
	if (IsValid(Parent))
	{
		Parent->InvokePostPipelineStageCallback(VCamera, Stage, State, DeltaTime);
	}
}

void UCinemachineVirtualCameraBaseComponent::InvokePrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraBaseComponent* VCamera, FCinemachineCameraState& State, float DeltaTime)
{
	for (int32 i = 0; i < Extensions.Num(); ++i)
	{
		UCinemachineExtension* Extension = Extensions[i];
		if (!IsValid(Extension))
		{
			Extensions.RemoveAt(i);
			--i;
		}
		else
		{
			Extension->PrePipelineMutateCameraStateCallback(VCamera, State, DeltaTime);
		}
	}

	UCinemachineVirtualCameraBaseComponent* Parent = Cast<UCinemachineVirtualCameraBaseComponent>(GetParentCamera().GetObject());
	if (IsValid(Parent))
	{
		Parent->InvokePrePipelineMutateCameraStateCallback(VCamera, State, DeltaTime);
	}
}

bool UCinemachineVirtualCameraBaseComponent::InvokeOnTransitionInExtensions(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime)
{
	bool bForceUpdate = false;
	for (int32 i = 0; i < Extensions.Num(); ++i)
	{
		UCinemachineExtension* Extension = Extensions[i];
		if (!IsValid(Extension))
		{
			Extensions.RemoveAt(i);
			--i;
		}
		else if (Extension->OnTransitionFromCamera(FromCamera, WorldUp, DeltaTime))
		{
			bForceUpdate = true;
		}
	}
	return bForceUpdate;
}

FCinemachineCameraState UCinemachineVirtualCameraBaseComponent::PullStateFromVirtualCamera(FVector WorldUp, FCinemachineLensSettings& Lens)
{
	FCinemachineCameraState State;
	State.RawLocation = GetComponentLocation();
	State.RawOrientation = GetComponentRotation();
	State.ReferenceUp = WorldUp;
	State.Lens = Lens;
	return State;
}

void UCinemachineVirtualCameraBaseComponent::UpdateTargetCache()
{
	USceneComponent* Target = ResolveFollow(GetFollow());
	bFollowTargetChanged = (Target != CachedFollowTarget);
	if (true == bFollowTargetChanged)
	{
		CachedFollowTarget = Target;
		CachedFollowTargetVCamera = nullptr;
		CachedFollowTargetGroup = nullptr;
		if (IsValid(CachedFollowTarget))
		{
			CachedFollowTargetVCamera = Cast<UCinemachineVirtualCameraBaseComponent>(CachedFollowTarget);
			CachedFollowTargetGroup = Cast<UCinemachineTargetGroupComponent>(CachedFollowTarget);
		}
	}
	Target = ResolveLookAt(GetLookAt());
	bLookAtTargetChanged = (Target != CachedLookAtTarget);
	if (true == bLookAtTargetChanged)
	{
		CachedLookAtTarget = Target;
		CachedLookAtTargetVCamera = nullptr;
		CachedLookAtTargetGroup = nullptr;
		if (IsValid(CachedLookAtTarget))
		{
			CachedLookAtTargetVCamera = Cast<UCinemachineVirtualCameraBaseComponent>(CachedLookAtTarget);
			CachedLookAtTargetGroup = Cast<UCinemachineTargetGroupComponent>(CachedLookAtTarget);
		}
	}
}

void UCinemachineVirtualCameraBaseComponent::UpdateStatusAsChild()
{
	bChildStatusUpdated = true;
	ParentCamera = Cast<UCinemachineVirtualCameraBaseComponent>(GetAttachParent());
}

void UCinemachineVirtualCameraBaseComponent::UpdateVcamPoolStatus()
{
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld());
	if (!IsValid(Subsystem))
	{
		return;
	}

	Subsystem->RemoveActiveCamera(this);
	if (!IsValid(GetParentCamera().GetObject()) && IsEnable())
	{
		Subsystem->AddActiveCamera(this);
	}

	QueuePriority = Priority;
}

void UCinemachineVirtualCameraBaseComponent::InvalidateCachedTargets()
{
	CachedFollowTarget = nullptr;
	CachedFollowTargetVCamera = nullptr;
	CachedFollowTargetGroup = nullptr;
	CachedLookAtTarget = nullptr;
	CachedLookAtTargetVCamera = nullptr;
	CachedLookAtTargetGroup = nullptr;
}
