

#include "CinemachineVirtualCameraBaseComponent.h"
#include "CinemachineExtension.h"
#include "CinemachineCoreSubSystem.h"
#include "CinemachineTargetGroupComponent.h"
#include "CinemachineInputAxisProviderInterface.h"
#include "Damper.h"
#include "VectorExtension.h"

UCinemachineVirtualCameraBaseComponent::UCinemachineVirtualCameraBaseComponent()
	: Priority(0)
	, ActivationId(0)
	, FollowTargetAttachment(0.0F)
	, LookAtTargetAttachment(0.0F)
	, StandbyUpdateMode(ECineamchineStandbyUpdateMode::Nerver)
	, bFollowTargetChanged(false)
	, bLookAtTargetChanged(false)
	, LookAtTarget(nullptr)
	, FollowTarget(nullptr)
	, InputAxisProvider(nullptr)
	, bCameraEnable(true)
{
	bCacheCameraEnable = bCameraEnable;

	PrimaryComponentTick.bCanEverTick = true;
}

void UCinemachineVirtualCameraBaseComponent::BeginPlay()
{
	Super::BeginPlay();
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
	SetEnable(false);
	Super::EndPlay(EndPlayReason);
}

void UCinemachineVirtualCameraBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TickType == ELevelTick::LEVELTICK_All)
	{
		if (Priority != QueuePriority)
		{
			UpdateVcamPoolStatus();
		}
	}
}

#if WITH_EDITOR
void UCinemachineVirtualCameraBaseComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld())
	{
		SetEnable(bCameraEnable);
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UCinemachineVirtualCameraBaseComponent::UpdateCameraState(FVector WorldUp, float DeltaTime)
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		World->GetSubsystem<UCinemachineCoreSubSystem>()->UpdateVirtualCamera(this, WorldUp, DeltaTime);
	}
}

void UCinemachineVirtualCameraBaseComponent::OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime)
{
	if (!IsEnable())
	{
		SetPreviousStateIsValid(false);
	}
}

void UCinemachineVirtualCameraBaseComponent::OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta)
{
	for (UCinemachineExtension* Extension : Extensions)
	{
		Extension->OnTargetObjectWarped(Target, LocationDelta);
	}
}

bool UCinemachineVirtualCameraBaseComponent::RequiresUserInput()
{
	for (UCinemachineExtension* Extension : Extensions)
	{
		if (Extension->RequiresUserInput())
		{
			return true;
		}
	}
	return false;
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

void UCinemachineVirtualCameraBaseComponent::AddExtension(UCinemachineExtension* Extension)
{
	if (Extensions.Contains(Extension))
	{
		return;
	}
	Extensions.Emplace(Extension);
}

void UCinemachineVirtualCameraBaseComponent::RemoveExtension(UCinemachineExtension* Extension)
{
	Extensions.Remove(Extension);
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

void UCinemachineVirtualCameraBaseComponent::MoveToTopOfPrioritySubqueue()
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

UCinemachineTargetGroupBaseComponent* UCinemachineVirtualCameraBaseComponent::GetFollowTargetGroup()
{
	return CachedFollowTargetGroup;
}

UCinemachineVirtualCameraBaseComponent* UCinemachineVirtualCameraBaseComponent::GetFollowTargetAsVCamera()
{
	return CachedFollowTargetVCamera;
}

UCinemachineTargetGroupBaseComponent* UCinemachineVirtualCameraBaseComponent::GetLookAtTargetGroup()
{
	return CachedLookAtTargetGroup;
}

UCinemachineVirtualCameraBaseComponent* UCinemachineVirtualCameraBaseComponent::GetLookAtTargetAsVCamera()
{
	return CachedLookAtTargetVCamera;
}

ICinemachineInputAxisProviderInterface* UCinemachineVirtualCameraBaseComponent::GetInputAxisProvider() const
{
	return InputAxisProvider;
}

void UCinemachineVirtualCameraBaseComponent::SetInputAxisProvider(TScriptInterface<ICinemachineInputAxisProviderInterface> InsertProvider)
{
	if (UObject* Object = InsertProvider.GetObject())
	{
		if (ICinemachineInputAxisProviderInterface* Provider = Cast<ICinemachineInputAxisProviderInterface>(Object))
		{
			InputAxisProvider = Provider;
		}
	}
}

void UCinemachineVirtualCameraBaseComponent::OnEnable()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	UCinemachineCoreSubSystem* SubSystem = World->GetSubsystem<UCinemachineCoreSubSystem>();
	if (!IsValid(SubSystem))
	{
		return;
	}

	UpdateSlaveStatus();
	UpdateVcamPoolStatus();
	if (!SubSystem->IsLive(this))
	{
		PreviousStateIsValid = false;
	}
	SubSystem->CameraEnabled(this);
	InvalidateCachedTargets();
}

void UCinemachineVirtualCameraBaseComponent::OnDisable()
{
	if (UWorld* World = GetWorld())
	{
		UpdateVcamPoolStatus();
		World->GetSubsystem<UCinemachineCoreSubSystem>()->CameraDisabled(this);
	}
}

void UCinemachineVirtualCameraBaseComponent::InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime)
{
	for (int32 i = 0; i < Extensions.Num(); ++i)
	{
		UCinemachineExtension* Extension = Extensions[i];
		if (!IsValid(Extension))
		{
			Extensions.RemoveAt(i);
			--i;
		}
		else if(Extension->IsEnable())
		{
			Extension->InvokePostPipelineStageCallback(VCamera, Stage, State, DeltaTime);
		}
	}
	UCinemachineVirtualCameraBaseComponent* Parent = GetParentCamera();
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
		else if (Extension->IsEnable())
		{
			Extension->PrePipelineMutateCameraStateCallback(VCamera, State, DeltaTime);
		}
	}
	UCinemachineVirtualCameraBaseComponent* Parent = GetParentCamera();
	if (IsValid(Parent))
	{
		Parent->InvokePrePipelineMutateCameraStateCallback(VCamera, State, DeltaTime);
	}
}

bool UCinemachineVirtualCameraBaseComponent::InvokeOnTransitionInExtensions(UCinemachineVirtualCameraBaseComponent* FromVCamera, FVector WorldUp, float DeltaTime)
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
		else if (Extension->IsEnable() && Extension->OnTransitionFromCamera(FromVCamera, WorldUp, DeltaTime))
		{
			bForceUpdate = true;
		}
	}
	return bForceUpdate;
}

void UCinemachineVirtualCameraBaseComponent::ApplyLocationBlendMethod(FCinemachineCameraState& State, ECinemachineBlendHint BlendHint)
{
	switch (BlendHint)
	{
	case ECinemachineBlendHint::SphericalLocation:
		State.BlendHint = static_cast<ECinemachineBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECinemachineBlendHintValue::SphericalLocationBlend));
		break;
	case ECinemachineBlendHint::CylindriacalLocation:
		State.BlendHint = static_cast<ECinemachineBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECinemachineBlendHintValue::CylindricalLocationBlend));
		break;
	case ECinemachineBlendHint::ScreenSpaceAimWhenTargetsDiffer:
		State.BlendHint = static_cast<ECinemachineBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECinemachineBlendHintValue::RadialAimBlend));
		break;
	}
}

UCinemachineBlend* UCinemachineVirtualCameraBaseComponent::CreateBlend(UObject* CameraA, UObject* CameraB, FCinemachineBlendDefinition BlendDefinition, UCinemachineBlend* ActiveBlend)
{
	if (BlendDefinition.Time <= 0.0F || (IsValid(CameraA) && IsValid(CameraB)))
	{
		BlendStartPosition = 0.0F;
		return nullptr;
	}

	if (IsValid(ActiveBlend))
	{
		if (false == ActiveBlend->IsComplete() && ActiveBlend->CameraA == CameraB && ActiveBlend->CameraB == CameraA)
		{
			float Progress = BlendStartPosition + (1.0F - BlendStartPosition) * ActiveBlend->TimeInBlend / ActiveBlend->Duration;
			BlendDefinition.Time *= Progress;
			BlendStartPosition = 1 - Progress;
		}
		else
		{
			BlendStartPosition = 0.0F;
		}

		if (Cast<UCinemachineVirtualCameraBaseComponent>(CameraB))
		{
			CameraA = nullptr;
		}
		else
		{
			UBlendSourceVirtualCamera* BlendSource = NewObject<UBlendSourceVirtualCamera>();
			BlendSource->Blend = ActiveBlend;
			CameraA = BlendSource;
		}
	}

	if (!IsValid(CameraA))
	{
		UStaticPointVirtualCamera* StaticCamera = NewObject<UStaticPointVirtualCamera>();
		StaticCamera->State = GetState();
		CameraA = StaticCamera;
	}

	UCinemachineBlend* NewBlend = NewObject<UCinemachineBlend>();
	NewBlend->CameraA = CameraA;
	NewBlend->CameraB = CameraB;
	NewBlend->BlendCurve = BlendDefinition.Curve;
	NewBlend->Duration = BlendDefinition.Time;
	NewBlend->TimeInBlend = 0.0F;

	return NewBlend;
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

void UCinemachineVirtualCameraBaseComponent::UpdateSlaveStatus()
{
	SlaveStatusUpdated = true;
	ParentCamera = Cast<UCinemachineVirtualCameraBaseComponent>(GetAttachParent());
}

void UCinemachineVirtualCameraBaseComponent::UpdateVcamPoolStatus()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	World->GetSubsystem<UCinemachineCoreSubSystem>()->RemoveActiveCamera(this);
	if (!IsValid(GetParentCamera()) && IsEnable())
	{
		World->GetSubsystem<UCinemachineCoreSubSystem>()->AddActiveCamera(this);
	}
	QueuePriority = GetPriority();
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
