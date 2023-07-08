

#include "CinemachineVirtualCameraComponent.h"
#include "UObject/Package.h"
#include "Camera/CinemachineCoreSubSystem.h"

UCinemachineVirtualCameraComponent::UCinemachineVirtualCameraComponent()
	: DefaultCachedLookAtTarget(nullptr)
	, DefaultCachedLookAtTargetVCamera(nullptr)
{
}

#if WITH_EDITOR
void UCinemachineVirtualCameraComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_PROPERTY(PropertyChangedEvent, UCinemachineVirtualCameraComponent, AimStageClass, AimStageTemplate, ECVStage::Aim);
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_PROPERTY(PropertyChangedEvent, UCinemachineVirtualCameraComponent, BodyStageClass, BodyStageTemplate, ECVStage::Body);
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_PROPERTY(PropertyChangedEvent, UCinemachineVirtualCameraComponent, NoiseStageClass, NoiseStageTemplate, ECVStage::Noise);
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UCinemachineVirtualCameraComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_CHAIN_PROPERTY(PropertyChangedEvent, UCinemachineVirtualCameraComponent, AimStageClass, AimStageTemplate, ECVStage::Aim);
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_CHAIN_PROPERTY(PropertyChangedEvent, UCinemachineVirtualCameraComponent, BodyStageClass, BodyStageTemplate, ECVStage::Body);
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_CHAIN_PROPERTY(PropertyChangedEvent, UCinemachineVirtualCameraComponent, NoiseStageClass, NoiseStageTemplate, ECVStage::Noise);
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
#endif

float UCinemachineVirtualCameraComponent::GetMaxDampTime()
{
	float MaxDamp = Super::GetMaxDampTime();
	for (UCinemachineBaseStage* CinemachineComponent : StagePipeline)
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

	bool bFollowIsValid = IsValid(GetFollow());
	bool bLookAtIsValid = IsValid(GetLookAt());

	if (bFollowIsValid && bLookAtIsValid)
	{
		SetWorldLocationAndRotation(VCameraState.RawLocation, VCameraState.RawOrientation);
	}
	else
	{
		if (bFollowIsValid)
		{
			SetWorldLocation(VCameraState.RawLocation);
		}
		if (bLookAtIsValid)
		{
			SetWorldRotation(VCameraState.RawOrientation);
		}
	}

	SetPreviousStateIsValid(true);
}

void UCinemachineVirtualCameraComponent::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	SetPreviousStateIsValid(true);
	SetWorldLocationAndRotation(Location, Rotation);
	VCameraState.RawLocation = Location;
	VCameraState.RawOrientation = Rotation;

	for (UCinemachineBaseStage* CinemachineComponent : StagePipeline)
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

	for (UCinemachineBaseStage* CinemachineComponent : StagePipeline)
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
	for (UCinemachineBaseStage* CinemachineComponent : StagePipeline)
	{
		if (!IsValid(CinemachineComponent))
		{
			continue;
		}
		if (CinemachineComponent->RequiresUserInput())
		{
			return true;
		}
	}
	return false;
}

UCinemachineBaseStage* UCinemachineVirtualCameraComponent::GetCinemachineStage(ECVStage Stage)
{
	for (UCinemachineBaseStage* CinemachineComponent : StagePipeline)
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

UCinemachineBaseStage* UCinemachineVirtualCameraComponent::AddCinemachineComponent(TSubclassOf<UCinemachineBaseStage> CreateComponent, FName ComponentName, UCinemachineBaseStage* Template)
{
	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (HasAllFlags(RF_Transient) || (IsValid(GetOwner()) && (GetOwner()->HasAllFlags(RF_Transient))))
	{
		Flags |= RF_Transient;
	}

	UCinemachineBaseStage* Component = NewObject<UCinemachineBaseStage>(this, CreateComponent, ComponentName, Flags, Template);
	if (IsValid(Component))
	{
		Component->Init(this);
		StagePipeline.Emplace(Component);
		StagePipeline.Sort([](UCinemachineBaseStage& Left, UCinemachineBaseStage& Right)
			{ return Left.GetStage() < Right.GetStage(); });
		return Component;
	}
	else
	{
		return nullptr;
	}
}

void UCinemachineVirtualCameraComponent::OnInitailize_Implementation()
{
	Super::OnInitailize_Implementation();

	if (AimStageClass != nullptr)
	{
		AddCinemachineComponent(AimStageClass, TEXT("AimStage(CV)"), AimStageTemplate);
	}

	if (BodyStageClass != nullptr)
	{
		AddCinemachineComponent(BodyStageClass, TEXT("BodyStage(CV)"), BodyStageTemplate);
	}

	if (NoiseStageClass != nullptr)
	{
		AddCinemachineComponent(NoiseStageClass, TEXT("NoiseStage(CV)"), NoiseStageTemplate);
	}
}

void UCinemachineVirtualCameraComponent::OnEnable()
{
	Super::OnEnable();
	VCameraState = PullStateFromVirtualCamera(FVector::UpVector, VCameraLens);
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

	InvokePrePipelineMutateCameraStateCallback(this, State, DeltaTime);

	bool HaveAim = false;
	if (StagePipeline.IsEmpty())
	{
		for (uint8 Stage = static_cast<uint8>(ECVStage::Body); Stage <= static_cast<uint8>(ECVStage::Finalize); ++Stage)
		{
			InvokePostPipelineStageCallback(this, static_cast<ECVStage>(Stage), State, DeltaTime);
		}
	}
	else
	{
		for (UCinemachineBaseStage* CinemachineComponent : StagePipeline)
		{
			if (IsValid(CinemachineComponent))
			{
				CinemachineComponent->PrePipelineMutateCameraState(State, DeltaTime);
			}
		}

		int32 ComponentIndex = 0;
		UCinemachineBaseStage* PostAimBody = nullptr;
		for (uint8 Stage = static_cast<uint8>(ECVStage::Body); Stage <= static_cast<uint8>(ECVStage::Finalize); ++Stage)
		{
			UCinemachineBaseStage* Select = StagePipeline.IsValidIndex(ComponentIndex) ? StagePipeline[ComponentIndex] : nullptr;
			if (IsValid(Select) && static_cast<ECVStage>(Stage) == Select->GetStage())
			{
				++ComponentIndex;
				if (static_cast<ECVStage>(Stage) == ECVStage::Body && Select->BodyAppliesAfterAim())
				{
					PostAimBody = Select;
					continue;
				}
				Select->MutateCameraState(State, DeltaTime);
				HaveAim = static_cast<ECVStage>(Stage) == ECVStage::Aim;
			}

			InvokePostPipelineStageCallback(this, static_cast<ECVStage>(Stage), State, DeltaTime);

			if (static_cast<ECVStage>(Stage) == ECVStage::Aim && IsValid(PostAimBody))
			{
				PostAimBody->MutateCameraState(State, DeltaTime);
				InvokePostPipelineStageCallback(this, ECVStage::Body, State, DeltaTime);
			}
		}
	}

	if (!HaveAim)
	{
		State.BlendHint = static_cast<ECVBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECVBlendHintValue::IgnoreLookAtTarget));
	}

	return State;
}
