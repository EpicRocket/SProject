

#include "CinemachineCamera.h"
#include "Camera/Core/CinemachineCoreSubSystem.h"
#include "Camera/Core/CinemachineBaseStage.h"

UCinemachineCamera::UCinemachineCamera()
{
	Pipeline.Reserve(static_cast<int32>(ECVStageType::Finalize) + 1);
}

#if WITH_EDITOR
void UCinemachineCamera::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCinemachineCamera, BodyStage))
	{
		if (IsValid(BodyStage) && BodyStage->GetStage() != ECVStageType::Body)
		{
			BodyStage = nullptr;
		}
	}
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCinemachineCamera, AimStage))
	{
		if (IsValid(AimStage) && AimStage->GetStage() != ECVStageType::Aim)
		{
			AimStage = nullptr;
		}
	}
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCinemachineCamera, NoiseStage))
	{
		if (IsValid(NoiseStage) && NoiseStage->GetStage() != ECVStageType::Noise)
		{
			NoiseStage = nullptr;
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UCinemachineCamera::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCinemachineCamera, BodyStage))
	{
		if (IsValid(BodyStage) && BodyStage->GetStage() != ECVStageType::Body)
		{
			BodyStage = nullptr;
		}
	}
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCinemachineCamera, AimStage))
	{
		if (IsValid(AimStage) && AimStage->GetStage() != ECVStageType::Aim)
		{
			AimStage = nullptr;
		}
	}
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCinemachineCamera, NoiseStage))
	{
		if (IsValid(NoiseStage) && NoiseStage->GetStage() != ECVStageType::Noise)
		{
			NoiseStage = nullptr;
		}
	}
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
#endif

USceneComponent* UCinemachineCamera::GetFollow() const
{
	return ResolveFollow(TrackingTarget);
}

void UCinemachineCamera::SetFollow(USceneComponent* FollowSceneComponent)
{
	TrackingTarget = FollowSceneComponent;
}

USceneComponent* UCinemachineCamera::GetLookAt() const
{
	return ResolveFollow(IsValid(LookAtTarget) ? LookAtTarget : TrackingTarget);
}

void UCinemachineCamera::SetLookAt(USceneComponent* LookAtSceneComponent)
{
	LookAtTarget = LookAtSceneComponent;
}

void UCinemachineCamera::InternalUpdateCameraState(FVector WorldUp, float DeltaTime)
{
	UpdateTargetCache();

	FollowTargetAttachment = 1.0F;
	LookAtTargetAttachment = 1.0F;

	if (DeltaTime < 0.0F)
	{
		SetPreviousStateIsValid(false);
	}

	CameraState = PullStateFromVirtualCamera(WorldUp, LensSettings);

	USceneComponent* Follow = GetFollow();
	USceneComponent* LookAt = GetLookAt();
	UCinemachineVirtualCameraBaseComponent* VCamera = GetLookAtTargetAsVCamera();
	if (IsValid(LookAt))
	{
		CameraState.ReferenceLookAt = IsValid(VCamera) ? VCamera->GetState().GetFinalLocation() : LookAt->GetComponentLocation();
	}
	InvokePipeline(CameraState, DeltaTime);
	CameraState.BlendHint = BlendHint;

	if (IsValid(Follow) && IsValid(LookAt))
	{
		SetWorldLocationAndRotation(CameraState.RawLocation, CameraState.RawOrientation);
	}
	else if (IsValid(Follow))
	{
		SetWorldLocation(CameraState.RawLocation);
	}
	else if (IsValid(LookAt))
	{
		SetWorldRotation(CameraState.RawOrientation);
	}

	SetPreviousStateIsValid(true);
}

void UCinemachineCamera::OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime)
{
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld());
	if (!IsValid(Subsystem))
	{
		return;
	}

	Super::OnTransitionFromCamera(FromCamera, WorldUp, DeltaTime);
	InvokeOnTransitionInExtensions(FromCamera, WorldUp, DeltaTime);
	bool bForceUpdate = false;

	bool bInheritLocation = (CameraState.BlendHint & static_cast<int32>(ECVBlendHintValue::InheritPosition)) != 0;
	if (bInheritLocation && IsCinemachineCameraValid(FromCamera) && !Subsystem->IsLiveInBlend(GetCinemachineCameraInterface(this)))
	{
		FCinemachineCameraState State = FromCamera->GetState();
		ForceCameraLocation(State.RawLocation, State.RawOrientation);
	}

	for (UCinemachineBaseStage* Stage : Pipeline)
	{
		if (IsValid(Stage) && Stage->OnTransitionFromCamera(FromCamera, WorldUp, DeltaTime))
		{
			bForceUpdate = true;
		}
	}

	if (!bForceUpdate)
	{
		UpdateCameraState(WorldUp, DeltaTime);
	}
	else
	{
		InternalUpdateCameraState(WorldUp, DeltaTime);
		InternalUpdateCameraState(WorldUp, DeltaTime);
	}
}

void UCinemachineCamera::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	SetPreviousStateIsValid(false);
	SetWorldLocationAndRotation(Location, Rotation);
	CameraState.RawLocation = Location;
	CameraState.RawOrientation = Rotation;

	for (UCinemachineBaseStage* Stage : Pipeline)
	{
		if (IsValid(Stage))
		{
			Stage->ForceCameraLocation(Location, Rotation);
		}
	}

	Super::ForceCameraLocation(Location, Rotation);
}

float UCinemachineCamera::GetMaxDampTime()
{
	float MaxDamping = Super::GetMaxDampTime();

	for (UCinemachineBaseStage* Stage : Pipeline)
	{
		if (IsValid(Stage))
		{
			MaxDamping = FMath::Max(MaxDamping, Stage->GetMaxDampTime());
		}
	}

    return MaxDamping;
}

UCinemachineBaseStage* UCinemachineCamera::GetStage(ECVStageType Stage)
{
	const int32 Index = static_cast<int32>(Stage);
    return Pipeline.IsValidIndex(Index) ? Pipeline[Index] : nullptr;
}

void UCinemachineCamera::OnInitailize()
{
	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (HasAllFlags(RF_Transient) || (IsValid(GetOwner()) && (GetOwner()->HasAllFlags(RF_Transient))))
	{
		Flags |= RF_Transient;
	}

	if (IsValid(BodyStage))
	{
		BodyStage->Init(this);
		Pipeline.Emplace(BodyStage);
	}

	if (IsValid(AimStage))
	{
		AimStage->Init(this);
		Pipeline.Emplace(AimStage);
	}

	if (IsValid(NoiseStage))
	{
		NoiseStage->Init(this);
		Pipeline.Emplace(NoiseStage);
	}

	Pipeline.Sort([](UCinemachineBaseStage& Left, UCinemachineBaseStage& Right)
		{ return Left.GetStage() < Right.GetStage(); });
}

void UCinemachineCamera::OnEnable()
{
	Super::OnEnable();

}

void UCinemachineCamera::OnDisable()
{
	Super::OnDisable();

}

FCinemachineCameraState UCinemachineCamera::InvokePipeline(FCinemachineCameraState& OutState, float DeltaTime)
{
	InvokePrePipelineMutateCameraStateCallback(this, OutState, DeltaTime);

	for (UCinemachineBaseStage* Stage : Pipeline)
	{
		if (IsValid(Stage) && Stage->IsStageValid())
		{
			Stage->PrePipelineMutateCameraState(OutState, DeltaTime);
		}
	}

	UCinemachineBaseStage* PostAimBody = nullptr;
	for (int32 i = 0; i < Pipeline.Num(); ++i)
	{
		ECVStageType StageType = static_cast<ECVStageType>(i);
		UCinemachineBaseStage* Stage = Pipeline[i];

		if (IsValid(Stage) && Stage->IsStageValid())
		{
			if (StageType == ECVStageType::Body && Stage->BodyAppliesAfterAim())
			{
				PostAimBody = Stage;
				continue;
			}
			Stage->MutateCameraState(OutState, DeltaTime);
		}
		InvokePostPipelineStageCallback(this, StageType, CameraState, DeltaTime);
		if (StageType == ECVStageType::Body && IsValid(PostAimBody))
		{
			PostAimBody->MutateCameraState(OutState, DeltaTime);
			InvokePostPipelineStageCallback(this, ECVStageType::Body, OutState, DeltaTime);
		}
	}

    return OutState;
}
