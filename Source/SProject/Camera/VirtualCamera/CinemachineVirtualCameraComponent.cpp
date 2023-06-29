

#include "CinemachineVirtualCameraComponent.h"
#include "Camera/CinemachineCoreSubSystem.h"

UCinemachineVirtualCameraComponent::UCinemachineVirtualCameraComponent()
	: DefaultCachedLookAtTarget(nullptr)
	, DefaultCachedLookAtTargetVCamera(nullptr)
{
}

void UCinemachineVirtualCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UCinemachineVirtualCameraComponent::GetMaxDampTime()
{
	float MaxDamp = Super::GetMaxDampTime();
	UpdateComponentPipeline();
	for (UCinemachineBaseComponent* CinemachineComponent : ComponentPipeline)
	{
		if (!IsValid(CinemachineComponent))
		{
			continue;
		}
		MaxDamp = FMath::Max(MaxDamp, CinemachineComponent->GetMaxDampTime());
	}
	return MaxDamp;
}

void UCinemachineVirtualCameraComponent::InternalUpdateCameraState(FVector WorldUp, float DeltaTime)
{
	UpdateTargetCache();

	VCameraState = CalculateNewState(WorldUp, DeltaTime);
	ApplyLocationBlendMethod(VCameraState, TransitionParameters.BlendHint);

	if (IsValid(GetFollow()))
	{
		SetWorldLocation(VCameraState.RawLocation);
	}
	if (IsValid(GetLookAt()))
	{
		SetWorldRotation(VCameraState.RawOrientation);
	}

	SetPreviousStateIsValid(true);
}

void UCinemachineVirtualCameraComponent::OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta)
{
	if (Target == GetFollow())
	{
		AddWorldOffset(LocationDelta);
		VCameraState.RawLocation += LocationDelta;
	}
	UpdateComponentPipeline();
	for (UCinemachineBaseComponent* CinemachineComponent : ComponentPipeline)
	{
		if (!IsValid(CinemachineComponent))
		{
			continue;
		}
		CinemachineComponent->OnTargetObjectWarped(Target, LocationDelta);
	}
	Super::OnTargetObjectWarped(Target, LocationDelta);
}

void UCinemachineVirtualCameraComponent::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	SetPreviousStateIsValid(true);
	SetWorldLocation(Location);
	SetWorldRotation(Rotation);
	VCameraState.RawLocation = Location;
	VCameraState.RawOrientation = Rotation;

	UpdateComponentPipeline();
	for (UCinemachineBaseComponent* CinemachineComponent : ComponentPipeline)
	{
		if (!IsValid(CinemachineComponent))
		{
			continue;
		}
		CinemachineComponent->ForceCameraLocation(Location, Rotation);
	}
	Super::ForceCameraLocation(Location, Rotation);
}

void UCinemachineVirtualCameraComponent::OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	UCinemachineCoreSubSystem* SubSystem = World->GetSubsystem<UCinemachineCoreSubSystem>();

	Super::OnTransitionFromCamera(FromCamera, WorldUp, DeltaTime);
	InvokeOnTransitionInExtensions(Cast<UCinemachineVirtualCameraBaseComponent>(FromCamera), WorldUp, DeltaTime);
	bool ForceUpdate = false;

	if (TransitionParameters.bInheritLocation && nullptr != FromCamera && SubSystem->IsLiveInBlend(this))
	{
		ForceCameraLocation(FromCamera->GetState().FinalLocation(), FromCamera->GetState().FinalOrientation());
	}

	UpdateComponentPipeline();

	for (UCinemachineBaseComponent* CinemachineComponent : ComponentPipeline)
	{
		if (!IsValid(CinemachineComponent))
		{
			continue;
		}
		if (CinemachineComponent->OnTransitionFromCamera(FromCamera, WorldUp, DeltaTime, TransitionParameters))
		{
			ForceUpdate = true;
		}
	}

	if (ForceUpdate)
	{
		InternalUpdateCameraState(WorldUp, DeltaTime);
	}
	else
	{
		UpdateCameraState(WorldUp, DeltaTime);
	}

	TransitionParameters.CameraActivatedEvent.Broadcast(this, Cast<UCinemachineVirtualCameraBaseComponent>(FromCamera));
}

bool UCinemachineVirtualCameraComponent::RequiresUserInput()
{
	if (Super::RequiresUserInput())
	{
		return true;
	}
	for (UCinemachineBaseComponent* CinemachineComponent : ComponentPipeline)
	{
		if (!IsValid(CinemachineComponent))
		{
			continue;
		}
		if (CinemachineComponent->RequriesUserInput())
		{
			return true;
		}
	}
	return false;
}

UCinemachineBaseComponent* UCinemachineVirtualCameraComponent::GetCinemachineComponent(ECinemachineStage Stage)
{
	for (UCinemachineBaseComponent* CinemachineComponent : ComponentPipeline)
	{
		if (!IsValid(CinemachineComponent))
		{
			continue;
		}
		if (CinemachineComponent->GetStage() == Stage)
		{
			return CinemachineComponent;
		}
	}
	return nullptr;
}

UCinemachineBaseComponent* UCinemachineVirtualCameraComponent::AddCinemachineComponent(TSubclassOf<UCinemachineBaseComponent> CreateComponent, FName ComponentName)
{
	UCinemachineBaseComponent* Component = NewObject<UCinemachineBaseComponent>(this, CreateComponent, ComponentName, EObjectFlags::RF_Transient);
	if (IsValid(Component))
	{
		Component->RegisterComponent();
		Component->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
	ComponentPipeline.Empty();
	return Component;
}

void UCinemachineVirtualCameraComponent::OnEnable()
{
	Super::OnEnable();
	VCameraState = PullStateFromVirtualCamera(FVector::UpVector, VCameraLens);
	ComponentPipeline.Empty();
}

void UCinemachineVirtualCameraComponent::UpdateComponentPipeline()
{
	TArray<UCinemachineBaseComponent*> NewComponentPipeline;
	for (USceneComponent* Child : GetAttachChildren())
	{
		UCinemachineBaseComponent* CinemachineComponent = Cast<UCinemachineBaseComponent>(Child);
		if (!IsValid(CinemachineComponent))
		{
			continue;
		}
		NewComponentPipeline.Add(CinemachineComponent);
	}
	NewComponentPipeline.Sort([](UCinemachineBaseComponent& Left, UCinemachineBaseComponent& Right)
							  { return Left.GetStage() < Right.GetStage(); });
	ComponentPipeline = NewComponentPipeline;
}

FCinemachineCameraState UCinemachineVirtualCameraComponent::CalculateNewState(FVector WorldUp, float DeltaTime)
{
	FollowTargetAttachment = 1.0F;
	LookAtTargetAttachment = 1.0F;

	FCinemachineCameraState State = PullStateFromVirtualCamera(WorldUp, VCameraLens);

	USceneComponent* Target = GetLookAt();
	if (Target != DefaultCachedLookAtTarget)
	{
		DefaultCachedLookAtTarget = Target;
		DefaultCachedLookAtTargetVCamera = Cast<UCinemachineVirtualCameraBaseComponent>(Target);
	}

	if (IsValid(Target))
	{
		if (IsValid(DefaultCachedLookAtTargetVCamera))
		{
			State.ReferenceLookAt = DefaultCachedLookAtTargetVCamera->GetState().FinalLocation();
		}
		else
		{
			State.ReferenceLookAt = Target->GetComponentLocation();
		}
	}

	UpdateComponentPipeline();

	InvokePrePipelineMutateCameraStateCallback(this, State, DeltaTime);

	bool HaveAim = false;
	if (ComponentPipeline.IsEmpty())
	{
		for (uint8 Stage = static_cast<uint8>(ECinemachineStage::Body); Stage <= static_cast<uint8>(ECinemachineStage::Finalize); ++Stage)
		{
			InvokePostPipelineStageCallback(this, static_cast<ECinemachineStage>(Stage), State, DeltaTime);
		}
	}
	else
	{
		for (UCinemachineBaseComponent* CinemachineComponent : ComponentPipeline)
		{
			if (IsValid(CinemachineComponent))
			{
				CinemachineComponent->PrePipelineMutateCameraState(State, DeltaTime);
			}
		}

		int32 ComponentIndex = 0;
		UCinemachineBaseComponent* PostAimBody = nullptr;
		for (uint8 Stage = static_cast<uint8>(ECinemachineStage::Body); Stage <= static_cast<uint8>(ECinemachineStage::Finalize); ++Stage)
		{
			UCinemachineBaseComponent* Select = ComponentPipeline.IsValidIndex(ComponentIndex) ? ComponentPipeline[ComponentIndex] : nullptr;
			if (IsValid(Select) && static_cast<ECinemachineStage>(Stage) == Select->GetStage())
			{
				++ComponentIndex;
				if (static_cast<ECinemachineStage>(Stage) == ECinemachineStage::Body && Select->BodyAppliesAfterAim())
				{
					PostAimBody = Select;
					continue;
				}
				Select->MutateCameraState(State, DeltaTime);
				HaveAim = static_cast<ECinemachineStage>(Stage) == ECinemachineStage::Aim;
			}

			InvokePostPipelineStageCallback(this, static_cast<ECinemachineStage>(Stage), State, DeltaTime);

			if (static_cast<ECinemachineStage>(Stage) == ECinemachineStage::Aim && IsValid(PostAimBody))
			{
				PostAimBody->MutateCameraState(State, DeltaTime);
				InvokePostPipelineStageCallback(this, ECinemachineStage::Body, State, DeltaTime);
			}
		}
	}

	if (!HaveAim)
	{
		State.BlendHint = static_cast<ECinemachineBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECinemachineBlendHintValue::IgnoreLookAtTarget));
	}

	return State;
}
