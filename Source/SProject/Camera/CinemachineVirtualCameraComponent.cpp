
#include "CinemachineVirtualCameraComponent.h"
#include "CinemachineExtension.h"
#include "CinemachineCoreSubSystem.h"
#include "CinemachineBrainComponent.h"
#include "CinemachineTargetGroupComponent.h"

static float Damp(float Initial, float DampTime, float DeltaTime)
{
	if (DampTime < 0.0001F || FMath::Abs(Initial) < 0.0001F)
	{
		return Initial;
	}
	if (DeltaTime < 0.0001F)
	{
		return 0.0F;
	}
	float K = -4.605170186F / DampTime;
	return Initial * (1 - FMath::Exp(-K * DeltaTime));
}

UCinemachineVirtualCameraComponent::UCinemachineVirtualCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCinemachineVirtualCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCinemachineVirtualCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCinemachineVirtualCameraComponent::UpdateCameraState(FVector WorldUp, float DeltaTime)
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		World->GetSubsystem<UCinemachineCoreSubSystem>()->UpdateVirtualCamera(this, WorldUp, DeltaTime);
	}
}

void UCinemachineVirtualCameraComponent::OnTransitionFromCamera(ICinemachineCameraInterface* InFromCamera, FVector WorldUp, float DeltaTime)
{
	if (!IsActive())
	{
		SetPreviousStateIsValid(false);
	}
}

void UCinemachineVirtualCameraComponent::OnTargetObjectWarped(AActor* InTarget, FVector InPositionDelta)
{
	for (UCinemachineExtension* Extension : Extensions)
	{
		Extension->OnTargetObjectWarped(InTarget, InPositionDelta);
	}
}

void UCinemachineVirtualCameraComponent::AddExtension(UCinemachineExtension* Extension)
{
	if (Extensions.Contains(Extension))
	{
		return;
	}
	Extensions.Emplace(Extension);
}

void UCinemachineVirtualCameraComponent::RemoveExtension(UCinemachineExtension* Extension)
{
	Extensions.Remove(Extension);
}

float UCinemachineVirtualCameraComponent::GetMaxDampTime() const
{
	float MaxDampTime = 0.0F;
	for (UCinemachineExtension* Extension : Extensions)
	{
		MaxDampTime = FMath::Max(MaxDampTime, Extension->GetMaxDampTime());
	}
	return MaxDampTime;
}

float UCinemachineVirtualCameraComponent::DetachedFollowTargetDamp(float Initial, float DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FMath::Max(1.0F, DampTime), DampTime, FollowTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, FollowTargetAttachment);
	return Damp(Initial, DampTime, DeltaTime);
}

FVector UCinemachineVirtualCameraComponent::DetachedFollowTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FVector::Max(FVector::OneVector, DampTime), DampTime, FollowTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, FollowTargetAttachment);
	for (int32 i = 0; i < 3; ++i)
	{
		Initial[i] = Damp(Initial[i], DampTime[i], DeltaTime);
	}
	return Initial;
}

FVector UCinemachineVirtualCameraComponent::DetachedFollowTargetDamp(FVector Initial, float DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FMath::Max(1.0F, DampTime), DampTime, FollowTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, FollowTargetAttachment);
	for (int32 i = 0; i < 3; ++i)
	{
		Initial[i] = Damp(Initial[i], DampTime, DeltaTime);
	}
	return Initial;
}

float UCinemachineVirtualCameraComponent::DetachedLookAtTargetDamp(float Initial, float DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FMath::Max(1.0F, DampTime), DampTime, LootAtTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, LootAtTargetAttachment);
	return Damp(Initial, DampTime, DeltaTime);
}

FVector UCinemachineVirtualCameraComponent::DetachedLookAtTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FVector::Max(FVector::OneVector, DampTime), DampTime, LootAtTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, LootAtTargetAttachment);
	for (int32 i = 0; i < 3; ++i)
	{
		Initial[i] = Damp(Initial[i], DampTime[i], DeltaTime);
	}
	return Initial;
}

FVector UCinemachineVirtualCameraComponent::DetachedLookAtTargetDamp(FVector Initial, float DampTime, float DeltaTime) const
{
	DampTime = FMath::Lerp(FMath::Max(1.0F, DampTime), DampTime, LootAtTargetAttachment);
	DeltaTime = FMath::Lerp(0.0F, DeltaTime, LootAtTargetAttachment);
	for (int32 i = 0; i < 3; ++i)
	{
		Initial[i] = Damp(Initial[i], DampTime, DeltaTime);
	}
	return Initial;
}

AActor* UCinemachineVirtualCameraComponent::ResolveFollow(AActor* LocalFollow)
{
	return !IsValid(LocalFollow) && IsValid(ParentCamera) ? ParentCamera->GetFollow() : LocalFollow;
}

AActor* UCinemachineVirtualCameraComponent::ResolveLookAt(AActor* LocalLookAt)
{
	return !IsValid(LocalLookAt) && IsValid(ParentCamera) ? ParentCamera->GetLookAt() : LocalLookAt;
}

void UCinemachineVirtualCameraComponent::MoveToTopOfPrioritySubqueue()
{
	UpdateVCameraStatus();
}

void UCinemachineVirtualCameraComponent::ForceCameraPosition(FVector Location, FQuat Rotation)
{
	for (UCinemachineExtension* Extension : Extensions)
	{
		Extension->ForceCameraLocation(Location, Rotation);
	}
}

UCinemachineTargetGroupBaseComponent* UCinemachineVirtualCameraComponent::GetFollowTargetGroup()
{
	return CachedFollowTargetGroup;
}

UCinemachineVirtualCameraComponent* UCinemachineVirtualCameraComponent::GetFollowTargetAsVCamera()
{
	return CachedFollowTargetVCamera;
}

UCinemachineTargetGroupBaseComponent* UCinemachineVirtualCameraComponent::GetLookAtTargetGroup()
{
	return CachedLookAtTargetGroup;
}

UCinemachineVirtualCameraComponent* UCinemachineVirtualCameraComponent::GetLookAtTargetAsVCamera()
{
	return CachedLookAtTargetVCamera;
}

void UCinemachineVirtualCameraComponent::InvokePostPipelineStageCallback(UCinemachineVirtualCameraComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime)
{
	for (int32 i = 0; i < Extensions.Num(); ++i)
	{
		UCinemachineExtension* Extension = Extensions[i];
		if (!IsValid(Extension))
		{
			Extensions.RemoveAt(i);
			--i;
		}
		else if (Extension->IsActive())
		{
			Extension->InvokePostPipelineStageCallback(VCamera, Stage, State, DeltaTime);
		}
	}
	UCinemachineVirtualCameraComponent* Parent = GetParentCamera();
	if (IsValid(Parent))
	{
		Parent->InvokePostPipelineStageCallback(VCamera, Stage, State, DeltaTime);
	}
}

void UCinemachineVirtualCameraComponent::InvokePrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraComponent* VCamera, FCinemachineCameraState& State, float DeltaTime)
{
	for (int32 i = 0; i < Extensions.Num(); ++i)
	{
		UCinemachineExtension* Extension = Extensions[i];
		if (!IsValid(Extension))
		{
			Extensions.RemoveAt(i);
			--i;
		}
		else if (Extension->IsActive())
		{
			Extension->PrePipelineMutateCameraStateCallback(VCamera, State, DeltaTime);
		}
	}
	UCinemachineVirtualCameraComponent* Parent = GetParentCamera();
	if (IsValid(Parent))
	{
		Parent->InvokePrePipelineMutateCameraStateCallback(VCamera, State, DeltaTime);
	}
}

bool UCinemachineVirtualCameraComponent::InvokeOnTransitionInExtensions(UCinemachineVirtualCameraComponent* FromVCamera, FVector WorldUp, float DeltaTime)
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
		else if (Extension->IsActive() && Extension->OnTransitionFromCamera(FromVCamera, WorldUp, DeltaTime))
		{
			bForceUpdate = true;
		}
	}
	return bForceUpdate;
}

void UCinemachineVirtualCameraComponent::ApplyLocationBlendMethod(FCinemachineCameraState& State, ECinemachineBlendHint BlendHint)
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

UCinemachineBlend* UCinemachineVirtualCameraComponent::CreateBlend(UObject* CameraA, UObject* CameraB, FCinemachineBlendDefinition BlendDefinition, UCinemachineBlend* ActiveBlend)
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

		if (Cast<UCinemachineVirtualCameraComponent>(CameraB))
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

FCinemachineCameraState UCinemachineVirtualCameraComponent::PullStateFromVirtualCamera(FVector WorldUp, FCinemachineLensSettings& Lens)
{
	FCinemachineCameraState State;

	State.RawLocation = GetComponentLocation();
	State.RawOrientation = GetComponentRotation();
	State.ReferenceUp = WorldUp;
	State.Lens = Lens;

	return State;
}

void UCinemachineVirtualCameraComponent::UpdateTargetCache()
{
	AActor* Target = ResolveFollow(GetFollow());
	bFollowTargetChanged = (Target != CachedFollowTarget);
	if (true == bFollowTargetChanged)
	{
		CachedFollowTarget = Target;
		CachedFollowTargetVCamera = nullptr;
		CachedFollowTargetGroup = nullptr;
		if (IsValid(CachedFollowTarget))
		{
			CachedFollowTargetVCamera = Cast<UCinemachineVirtualCameraComponent>(CachedFollowTarget->GetComponentByClass(UCinemachineVirtualCameraComponent::StaticClass()));
			CachedFollowTargetGroup = Cast<UCinemachineTargetGroupComponent>(CachedFollowTarget->GetComponentByClass(UCinemachineTargetGroupComponent::StaticClass()));
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
			CachedLookAtTargetVCamera = Cast<UCinemachineVirtualCameraComponent>(CachedLookAtTarget->GetComponentByClass(UCinemachineVirtualCameraComponent::StaticClass()));
			CachedLookAtTargetGroup = Cast<UCinemachineTargetGroupComponent>(CachedLookAtTarget->GetComponentByClass(UCinemachineTargetGroupComponent::StaticClass()));
		}
	}
}

void UCinemachineVirtualCameraComponent::UpdateSlaveStatus()
{
	SlaveStatusUpdated = true;
	ParentCamera = Cast<UCinemachineVirtualCameraComponent>(GetAttachParent());
}

void UCinemachineVirtualCameraComponent::UpdateVCameraStatus()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	World->GetSubsystem<UCinemachineCoreSubSystem>()->RemoveActiveCamera(this);
	if (!IsValid(GetParentCamera()) && IsActive())
	{
		World->GetSubsystem<UCinemachineCoreSubSystem>()->AddActiveCamera(this);
	}
	QueuePriority = GetPriority();
}

void UCinemachineVirtualCameraComponent::InvalidateCachedTargets()
{
	CachedFollowTarget = nullptr;
	CachedFollowTargetVCamera = nullptr;
	CachedFollowTargetGroup = nullptr;
	CachedLookAtTarget = nullptr;
	CachedLookAtTargetVCamera = nullptr;
	CachedLookAtTargetGroup = nullptr;
}
