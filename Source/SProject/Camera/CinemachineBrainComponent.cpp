

#include "CinemachineBrainComponent.h"
#include "Camera/CameraComponent.h"
#include "CinemachineVirtualCameraBaseComponent.h"
#include "CinemachineCoreSubSystem.h"

//! UCinemachineBrainFrame

FCVBrainFrame::FCVBrainFrame(UObject* Outer)
{
	if (IsValid(Outer))
	{
		EObjectFlags Flags = RF_Public;
		Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
		if (Outer->HasAllFlags(RF_Transient))
		{
			Flags |= RF_Transient;
		}

		Blend = NewObject<UCinemachineBlend>(Outer, TEXT("Blend(CV)"), Flags);
		WorkingBlend = NewObject<UCinemachineBlend>(Outer, TEXT("WorkingBlend(CV)"), Flags);
		WorkingBlendSource = NewObject<UBlendSourceVirtualCamera>(Outer, TEXT("WorkingBlendSource(CV)"), Flags);
	}
}

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

	// TODO: [Camera]에디터에서도 사용 가능하도록 수정 필요
	if (TickType == ELevelTick::LEVELTICK_All)
	{
		ManualUpdate(DeltaTime);
	}
}

#if WITH_EDITOR
void UCinemachineBrainComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld())
	{
		SetEnable(bBrainEnabled);
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

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
	CurrentLiveCameras = NewObject<UCinemachineBlend>(this, TEXT("CurrentLiveCameras(CV)"), Flags);
	OutputCamera = IsValid(TargetOverride) ? TargetOverride : Cast<UCameraComponent>(GetAttachParent());

	OnInitailize();
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
		LastFrameUpdated = -1;
		UpdateVirtualCameras(-1.0F);
	}
	else
	{
		OnDisable();
		SetComponentTickEnabled(false);
	}
}

UCameraComponent* UCinemachineBrainComponent::GetOutputCamera()
{
	if (!IsValid(OutputCamera))
	{
		OutputCamera = IsValid(TargetOverride) ? TargetOverride : Cast<UCameraComponent>(GetAttachParent());
	}
	return OutputCamera;
}

FVector UCinemachineBrainComponent::DefaultWorldUp() const
{
	return IsValid(WorldUpOverride) ? WorldUpOverride->GetUpVector() : FVector::UpVector;
}

void UCinemachineBrainComponent::ManualUpdate(float DeltaTime)
{
	LastFrameUpdated = GFrameCounter;
	UpdateFrame0(DeltaTime);
	ComputeCurrentBlend(CurrentLiveCameras, 0);
	UpdateVirtualCameras(DeltaTime);
	ProcessActiveCamera(DeltaTime);
}

UCinemachineVirtualCameraBaseComponent* UCinemachineBrainComponent::GetActiveVirtualCamera()
{
	if (!IsValid(CurrentLiveCameras))
	{
		return nullptr;
	}
	UObject* VCamera = CurrentLiveCameras->CameraB;
	while (IsValid(VCamera))
	{
		UBlendSourceVirtualCamera* BlendSource = Cast<UBlendSourceVirtualCamera>(VCamera);
		if (!IsValid(BlendSource))
		{
			break;
		}
		UCinemachineBlend* Blend = BlendSource->Blend;
		if (!IsValid(Blend))
		{
			break;
		}
		VCamera = Blend->CameraB;
	}
	return Cast<UCinemachineVirtualCameraBaseComponent>(VCamera);
}

bool UCinemachineBrainComponent::IsLiveInBlend(ICinemachineCameraInterface* ICamera)
{
	if (nullptr == ICamera || !IsValid(CurrentLiveCameras))
	{
		return false;
	}

	if (!IsValid(CurrentLiveCameras))
	{
		return false;
	}

	if (ICamera == Cast<ICinemachineCameraInterface>(CurrentLiveCameras->CameraA))
	{
		return true;
	}

	UBlendSourceVirtualCamera* BlendSource = Cast<UBlendSourceVirtualCamera>(CurrentLiveCameras->CameraA);
	if (IsValid(BlendSource))
	{
		UCinemachineBlend* Blend = BlendSource->Blend;
		if (IsValid(Blend) && Blend->Uses(ICamera))
		{
			return true;
		}
	}

	ICinemachineCameraInterface* Parent = ICamera->GetParentCamera();
	if (nullptr != Parent && Parent->IsLiveChild(ICamera, false))
	{
		return IsLiveInBlend(Parent);
	}

	return false;
}

bool UCinemachineBrainComponent::IsLive(ICinemachineCameraInterface* ICamera, bool DominantChildOnly)
{
	if (nullptr == ICamera || !IsValid(CurrentLiveCameras))
	{
		return false;
	}

	if (CurrentLiveCameras->Uses(ICamera))
	{
		return true;
	}

	ICinemachineCameraInterface* Parent = ICamera->GetParentCamera();
	while (nullptr != Parent && Parent->IsLiveChild(ICamera, DominantChildOnly))
	{
		if (CurrentLiveCameras->Uses(Parent))
		{
			return true;
		}
		ICamera = Parent;
		Parent = ICamera->GetParentCamera();
	}
	return false;
}

void UCinemachineBrainComponent::ComputeCurrentBlend(UCinemachineBlend* OutputBlend, int32 NumTopLayersToExclude)
{
	if (FrameStack.Num() == 0)
	{
		FrameStack.Emplace(FCVBrainFrame(this));
	}

	int32 LastActive = 0;
	int32 TopLayer = FMath::Max(1, FrameStack.Num() - NumTopLayersToExclude);
	for (int32 i = 0; i < TopLayer; ++i)
	{
		FCVBrainFrame& Frame = FrameStack[i];

		if (i == 0 || Frame.bActive)
		{
			Frame.WorkingBlend->CameraA = Frame.Blend->CameraA;
			Frame.WorkingBlend->CameraB = Frame.Blend->CameraB;
			Frame.WorkingBlend->BlendCurve = Frame.Blend->BlendCurve;
			Frame.WorkingBlend->TimeInBlend = Frame.Blend->TimeInBlend;
			Frame.WorkingBlend->Duration = Frame.Blend->Duration;
			if (i > 0 && !Frame.Blend->IsComplete())
			{
				if (!IsValid(Frame.WorkingBlend->CameraA))
				{
					if (FrameStack[LastActive].Blend->IsComplete())
					{
						Frame.WorkingBlend->CameraA = FrameStack[LastActive].Blend->CameraB;
					}
					else
					{
						Frame.WorkingBlendSource->Blend = FrameStack[LastActive].WorkingBlend;
						Frame.WorkingBlend->CameraA = Frame.WorkingBlendSource;
					}
				}
				else if (!IsValid(Frame.WorkingBlend->CameraB))
				{
					if (FrameStack[LastActive].Blend->IsComplete())
					{
						Frame.WorkingBlend->CameraB = FrameStack[LastActive].Blend->CameraB;
					}
					else
					{
						Frame.WorkingBlendSource->Blend = FrameStack[LastActive].WorkingBlend;
						Frame.WorkingBlend->CameraB = Frame.WorkingBlendSource;
					}
				}
			}
			LastActive = i;
		}
	}
	if (IsValid(OutputBlend))
	{
		UCinemachineBlend* WorkingBlend = FrameStack[LastActive].WorkingBlend;
		OutputBlend->CameraA = WorkingBlend->CameraA;
		OutputBlend->CameraB = WorkingBlend->CameraB;
		OutputBlend->BlendCurve = WorkingBlend->BlendCurve;
		OutputBlend->TimeInBlend = WorkingBlend->TimeInBlend;
		OutputBlend->Duration = WorkingBlend->Duration;
	}
}

bool UCinemachineBrainComponent::IsBlending() const
{
	return IsValid(GetActiveBlend());
}

UCinemachineBlend* UCinemachineBrainComponent::GetActiveBlend() const
{
	if (!IsValid(CurrentLiveCameras))
	{
		return nullptr;
	}
	if (!IsValid(CurrentLiveCameras->CameraA) || CurrentLiveCameras->IsComplete())
	{
		return nullptr;
	}
	return CurrentLiveCameras;
}

void UCinemachineBrainComponent::SetActiveBlend(UCinemachineBlend* Value)
{
	if (!IsValid(Value))
	{
		FrameStack[0].Blend->Duration = 0;
	}
	else
	{
		FrameStack[0].Blend = Value;
	}
}

void UCinemachineBrainComponent::OnInitailize()
{
	LastFrameUpdated = -1;
	UpdateVirtualCameras(-1.0F);
}

void UCinemachineBrainComponent::OnEnable()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (FrameStack.IsEmpty())
	{
		FrameStack.Emplace(FCVBrainFrame(this));
	}
	World->GetSubsystem<UCinemachineCoreSubSystem>()->AddActiveBrain(this);


	if (IsValid(GetOutputCamera()))
	{
		CachedOutputCameraLocation = OutputCamera->GetComponentLocation();
		CachedOutputCameraRotation = OutputCamera->GetComponentRotation();
		OutputCamera->SetAbsolute(true, true, true);
	}
}

void UCinemachineBrainComponent::OnDisable()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	World->GetSubsystem<UCinemachineCoreSubSystem>()->RemoveActiveBrain(this);
	FrameStack.Empty();

	if (IsValid(GetOutputCamera()))
	{
		OutputCamera->SetAbsolute(false, false, false);
		OutputCamera->SetWorldLocationAndRotation(CachedOutputCameraLocation, CachedOutputCameraRotation);
	}
}

void UCinemachineBrainComponent::UpdateVirtualCameras(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	World->GetSubsystem<UCinemachineCoreSubSystem>()->UpdateAllActiveVirtualCameras(DefaultWorldUp(), DeltaTime);

	if (IsValid(CurrentLiveCameras))
	{
		CurrentLiveCameras->UpdateCameraState(DefaultWorldUp(), DeltaTime);
	}
}

int32 UCinemachineBrainComponent::GetBrainFrame(int32 WithId)
{
	for (int32 i = FrameStack.Num() - 1; i > 0; --i)
	{
		if (FrameStack[i].Id == WithId)
		{
			return i;
		}
	}
	FCVBrainFrame NewFrame(this);
	NewFrame.Id = WithId;
	FrameStack.Emplace(NewFrame);
	return FrameStack.Num() - 1;
}

void UCinemachineBrainComponent::ProcessActiveCamera(float DeltaTime)
{
	UCinemachineVirtualCameraBaseComponent* ActiveCamera = GetActiveVirtualCamera();
	if (!IsValid(ActiveCamera))
	{
		USceneComponent* SceneComponent = GetOutputCamera();
		if (!IsValid(SceneComponent))
		{
			return;
		}
		FCinemachineCameraState State;
		State.RawLocation = SceneComponent->GetComponentLocation();
		State.RawOrientation = SceneComponent->GetComponentRotation();
		State.Lens = FCinemachineLensSettings::FromCamera(GetOutputCamera());
		State.BlendHint = static_cast<ECVBlendHintValue>(static_cast<int32>(ECVBlendHintValue::NoTransform) | static_cast<int32>(ECVBlendHintValue::NoLens));
		PushStateToCamera(State);
	}
	else
	{
		if (ActiveCamera != ActiveCameraPreviousFrame)
		{
			ActiveCamera->OnTransitionFromCamera(ActiveCameraPreviousFrame, DefaultWorldUp(), DeltaTime);
			CameraActivatedEvent.Broadcast(ActiveCamera, ActiveCameraPreviousFrame);

			if (!IsBlending() || (IsValid(ActiveCameraPreviousFrame) && !GetActiveBlend()->Uses(ActiveCameraPreviousFrame)))
			{
				CameraCutEvent.Broadcast(this);
				if (UWorld* World = GetWorld())
				{
					World->GetSubsystem<UCinemachineCoreSubSystem>()->CameraCutEvent.Broadcast(this);
				}
			}
			ActiveCamera->UpdateCameraState(DefaultWorldUp(), DeltaTime);
		}
		if (IsValid(CurrentLiveCameras))
		{
			PushStateToCamera(CurrentLiveCameras->GetState());
		}
	}
	ActiveCameraPreviousFrame = ActiveCamera;
}

void UCinemachineBrainComponent::UpdateFrame0(float DeltaTime)
{
	if (FrameStack.IsEmpty())
	{
		FrameStack.Emplace(FCVBrainFrame(this));
	}

	FCVBrainFrame& Frame = FrameStack[0];
	UCinemachineVirtualCameraBaseComponent* ActiveCamera = TopCameraFromPriorityQueue();
	ICinemachineCameraInterface* OutGoingCamera = Cast<ICinemachineCameraInterface>(Frame.Blend->CameraB);

	if (ActiveCamera != OutGoingCamera)
	{
		if (IsValid(ActiveCamera) && OutGoingCamera != nullptr && DeltaTime > 0.0F)
		{
			FCinemachineBlendDefinition BlendDefinition = LookupBlend(Cast<UObject>(OutGoingCamera), Cast<UObject>(ActiveCamera));
			float BlendDuration = BlendDefinition.Time;
			float BlendStartPosition = 0.0F;

			if (BlendDuration > UE_KINDA_SMALL_NUMBER)
			{
				if (Frame.Blend->IsComplete())
				{
					Frame.Blend->CameraA = Cast<UObject>(OutGoingCamera);
				}
				else
				{
					ICinemachineCameraInterface* ICameraA = Cast<ICinemachineCameraInterface>(Frame.Blend->CameraA);
					ICinemachineCameraInterface* ICameraB = Cast<ICinemachineCameraInterface>(Frame.Blend->CameraB);

					UBlendSourceVirtualCamera* BlendSourceVCamera = Cast<UBlendSourceVirtualCamera>(ICameraA);
					if (((ICameraA == ActiveCamera) || (IsValid(BlendSourceVCamera) && BlendSourceVCamera->Blend->CameraB == ActiveCamera)) && ICameraB == OutGoingCamera)
					{
						float Alpha = Frame.Blend->Duration > UE_KINDA_SMALL_NUMBER ? Frame.Blend->TimeInBlend / Frame.Blend->Duration : 0.0F;
						float Progress = Frame.BlendStartPosition + (1 - Frame.BlendStartPosition) * Alpha;
						BlendDuration *= Progress;
						BlendStartPosition = 1 - Progress;
					}

					EObjectFlags Flags = RF_Public;
					Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
					if (HasAllFlags(RF_Transient))
					{
						Flags |= RF_Transient;
					}

					UCinemachineBlend* NewBlend = NewObject<UCinemachineBlend>(this, TEXT("NewBlend(CV)"), Flags);
					NewBlend->CameraA = Frame.Blend->CameraA;
					NewBlend->CameraB = Frame.Blend->CameraB;
					NewBlend->BlendCurve = Frame.Blend->BlendCurve;
					NewBlend->Duration = Frame.Blend->Duration;
					NewBlend->TimeInBlend = Frame.Blend->TimeInBlend;

					UBlendSourceVirtualCamera* NewBlendSourceVCamera = NewObject<UBlendSourceVirtualCamera>(this, TEXT("NewBlendSourceVCamera(CV)"), Flags);
					NewBlendSourceVCamera->Blend = NewBlend;

					Frame.Blend->CameraA = NewBlendSourceVCamera;
				}
			}

			Frame.Blend->BlendCurve = BlendDefinition.Curve;
			Frame.Blend->Duration = BlendDuration;
			Frame.Blend->TimeInBlend = 0.0F;
			Frame.BlendStartPosition = BlendStartPosition;
		}
		Frame.Blend->CameraB = ActiveCamera;
	}

	if (IsValid(Frame.Blend->CameraA))
	{
		Frame.Blend->TimeInBlend += (DeltaTime >= 0.0F) ? DeltaTime : Frame.Blend->Duration;
		if (Frame.Blend->IsComplete())
		{
			Frame.Blend->CameraA = nullptr;
			Frame.Blend->Duration = 0.0F;
			Frame.Blend->TimeInBlend = 0.0F;
		}
	}
}

UCinemachineVirtualCameraBaseComponent* UCinemachineBrainComponent::TopCameraFromPriorityQueue()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}

	UCinemachineCoreSubSystem* SubSystem = World->GetSubsystem<UCinemachineCoreSubSystem>();
	for (int32 i = 0; i < SubSystem->GetVirtualCameraCount(); ++i)
	{
		UCinemachineVirtualCameraBaseComponent* VCamera = SubSystem->GetVirtualCamera(i);
		if (IsValid(VCamera))
		{
			return VCamera;
		}
	}

	return nullptr;
}

FCinemachineBlendDefinition UCinemachineBrainComponent::LookupBlend(UObject* FromKey, UObject* ToKey)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return DefaultBlend;
	}

	UCinemachineCoreSubSystem* SubSystem = World->GetSubsystem<UCinemachineCoreSubSystem>();
	if (!SubSystem->OnBlendOverride.IsBound())
	{
		return DefaultBlend;
	}

	return SubSystem->OnBlendOverride.Execute(FromKey, ToKey, DefaultBlend, this);
}

void UCinemachineBrainComponent::PushStateToCamera(FCinemachineCameraState State)
{
	CurrentCameraState = State;
	UCameraComponent* Camera = GetOutputCamera();
	if (IsValid(Camera))
	{
		int32 BlendHint = static_cast<int32>(CurrentCameraState.BlendHint);

		bool bLocation = (BlendHint & static_cast<int32>(ECVBlendHintValue::NoLocation)) == 0;
		bool bOrientation = (BlendHint & static_cast<int32>(ECVBlendHintValue::NoOrientation)) == 0;
		if (bLocation && bOrientation)
		{
			Camera->SetWorldLocationAndRotation(CurrentCameraState.FinalLocation(), CurrentCameraState.FinalOrientation());
		}
		else
		{
			if (bLocation)
			{
				Camera->SetWorldLocation(CurrentCameraState.FinalLocation());
			}
			if (bOrientation)
			{
				Camera->SetWorldRotation(CurrentCameraState.FinalOrientation());
			}
		}
		if ((BlendHint & static_cast<int32>(ECVBlendHintValue::NoLens)) == 0)
		{
			if (IsValid(Camera))
			{
				Camera->FieldOfView = CurrentCameraState.Lens.FieldOfView;
				Camera->AspectRatio = CurrentCameraState.Lens.AspectRatio;
			}
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetSubsystem<UCinemachineCoreSubSystem>()->CameraUpdatedEvent.Broadcast(this);
	}
}
