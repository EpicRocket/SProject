

#include "CinemachineCameraBlendStack.h"
#include "CinemachineCoreSubSystem.h"
#include "CinemachineVirtualCameraBaseComponent.h"

EAlphaBlendOption GDefaultCinemachineBlendStackAlphaOption = EAlphaBlendOption::Linear;
UCurveFloat* GDefaultCinemachineBlendStackCustomCurve = nullptr;

#define LOCTEXT_NAMESPACE "CinemachineCameraBlendStack"
static UCVBlendStackFrame* CreateBlendStackFrame(UObject* Outer, bool bDoInitailize = true)
{
	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (IsValid(Outer))
	{
		if (Outer->HasAllFlags(RF_Transient))
		{
			Flags |= RF_Transient;
		}
	}

	UCVBlendStackFrame* NewBlendStackFrame = NewObject<UCVBlendStackFrame>(Outer, NAME_None, Flags);
	if (bDoInitailize)
	{
		NewBlendStackFrame->Init();
	}
	return NewBlendStackFrame;
}

static bool AdvanceBlend(UCinemachineBlend* Blend, float DeltaTime)
{
	bool bCompleted = false;
	if (!IsValid(Blend))
	{
		return bCompleted;
	}
	
	if (IsValid(Blend->CameraA))
	{
		Blend->TimeInBlend += DeltaTime >= 0.0F ? DeltaTime : Blend->Duration;
		if (Blend->IsComplete())
		{
			Blend->ClearBlend();
			bCompleted = true;
		}
		else if (UCinemachineNestedBlendSource* NestedBlendSource = Cast<UCinemachineNestedBlendSource>(Blend->CameraA))
		{
			AdvanceBlend(NestedBlendSource->Blend, DeltaTime);
		}
	}

	return bCompleted;
}
#undef LOCTEXT_NAMESPACE

//! UCVSnapshotBlendSource

void UCVSnapshotBlendSource::TakeSnapshot(ICinemachineCameraInterface* Source)
{
	if (Source != nullptr)
	{
		State = Source->GetState();
		SnapshotName = FString::Printf(TEXT("*%s"), *Source->GetCameraName());
	}
	else
	{
		SnapshotName = TEXT("(NULL)");
	}
	State.BlendHint &= ~static_cast<int32>(ECVBlendHintValue::FreezeWhenBlendingOut);
}

//! UCVBlendStackFrame

void UCVBlendStackFrame::Init()
{
	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (HasAllFlags(RF_Transient))
	{
		Flags |= RF_Transient;
	}
	Blend = NewObject<UCinemachineBlend>(this, NAME_None, Flags);
	Source = NewObject<UCinemachineBlend>(this, NAME_None, Flags);
	Snapshot = NewObject<UCVSnapshotBlendSource>(this, NAME_None, Flags);
}

ICinemachineCameraInterface* UCVBlendStackFrame::GetSnapshotIfAppropriate(ICinemachineCameraInterface* ICamera, float Weight)
{
	bool bNotFreezeWhenBlendingOut = IsCinemachineCameraValid(ICamera) && (ICamera->GetState().BlendHint & static_cast<int32>(ECVBlendHintValue::FreezeWhenBlendingOut)) == 0;
	if (bNotFreezeWhenBlendingOut)
	{
		if (IsValid(Snapshot))
		{
			Snapshot->TakeSnapshot(nullptr);
		}
		return ICamera;
	}

	ICinemachineCameraInterface* SnapshotCamera = Cast<ICinemachineCameraInterface>(SnapshotSource);
	if (SnapshotCamera != ICamera || SnapshotBlendWeight > Weight)
	{
		Snapshot->TakeSnapshot(ICamera);
		SnapshotSource = Cast<UObject>(ICamera);
		SnapshotBlendWeight = Weight;
	}

	return Snapshot;
}

//! UCinemachineCameraBlendStack

int32 UCinemachineCameraBlendStack::SetCameraOverride(int32 OverrideId, int32 Priority, ICinemachineCameraInterface* CameraA, ICinemachineCameraInterface* CameraB, float WeightB, float DeltaTime)
{
	if (OverrideId < 0)
	{
		OverrideId = NextFrameId++;
	}

	int32 FrameIndex = FindFrame(OverrideId, Priority);
	if (!FrameStack.IsValidIndex(FrameIndex))
	{
		return OverrideId;
	}

	UCVBlendStackFrame* StackFrame = FrameStack[FrameIndex];
	if (!IsValid(StackFrame))
	{
		return OverrideId;
	}

	StackFrame->Source->TimeInBlend = WeightB;

	UObject* CameraAObject = Cast<UObject>(CameraA);
	UObject* CameraBObject = Cast<UObject>(CameraB);
	if (StackFrame->Source->CameraA != CameraAObject || StackFrame->Source->CameraB != CameraBObject)
	{
		StackFrame->Source->CameraA = CameraAObject;
		StackFrame->Source->CameraB = CameraBObject;
	}

	return OverrideId;
}

void UCinemachineCameraBlendStack::ReleaseCameraOverride(int32 OverrideId)
{
	for (int Index = FrameStack.Num() - 1; Index > 0; --Index)
	{
		if (FrameStack[Index]->Id == OverrideId)
		{
			FrameStack.RemoveAt(Index);
			return;
		}
	}
}

void UCinemachineCameraBlendStack::Init()
{
	if (bIsInitialized)
	{
		return;
	}
	bIsInitialized = true;

	OnInitailize();
	bCacheEnable = false;
	SetEnable(true);
}

int32 UCinemachineCameraBlendStack::FindFrame(int32 WithId, int32 Priority)
{
	int32 Count = FrameStack.Num();
	int32 Index = INDEX_NONE;
	for (Index = Count - 1; Index > 0; --Index)
	{
		if (FrameStack[Index]->Id == WithId)
		{
			return Index;
		}
	}

	for (Index = 1; Index < Count; ++Index)
	{
		if (FrameStack[Index]->Priority > Priority)
		{
			break;
		}
	}

	UCVBlendStackFrame* NewFrame = CreateBlendStackFrame(this, false);
	NewFrame->Id = WithId;
	NewFrame->Priority = Priority;
	NewFrame->Init();

	NewFrame->Source->Duration = 1.0F;
	NewFrame->Source->BlendCurve.SetCustomCurve(GDefaultCinemachineBlendStackCustomCurve);
	NewFrame->Source->BlendCurve.SetBlendOption(GDefaultCinemachineBlendStackAlphaOption);
	FrameStack.Insert(NewFrame, Index);

	return Index;
}

void UCinemachineCameraBlendStack::SetEnable(bool bEnable)
{
	if (bCacheEnable == bEnable)
	{
		return;
	}
	bCacheEnable = bEnable;
	if (bCacheEnable)
	{
		OnEnable();
	}
	else
	{
		OnDisable();
	}
}

bool UCinemachineCameraBlendStack::IsInitialized() const
{
	return bIsInitialized && FrameStack.Num() > 0;
}

void UCinemachineCameraBlendStack::ResetRootFrame()
{
	if (FrameStack.IsEmpty())
	{
		FrameStack.Add(CreateBlendStackFrame(this));
	}
	else
	{
		if (UCVBlendStackFrame* RootFrame = FrameStack[0])
		{
			RootFrame->Blend->ClearBlend();
			RootFrame->Blend->CameraB = nullptr;
			RootFrame->Source->ClearBlend();
			RootFrame->Source->CameraB = nullptr;
		}
	}
}

void UCinemachineCameraBlendStack::UpdateRootFrame(ICinemachineMixerInterface* Context, ICinemachineCameraInterface* ActiveCamera, FVector Up, float DeltaTime)
{
	UObject* ActiveCameraObject = Cast<UObject>(ActiveCamera);

	if (FrameStack.IsEmpty())
	{
		FrameStack.Add(CreateBlendStackFrame(this));
	}

	UCVBlendStackFrame* RootFrame = FrameStack[0];
	if(!IsValid(RootFrame) || !IsValid(RootFrame->Blend))
	{
		return;
	}
	if (!IsValid(RootFrame->Source))
	{
		RootFrame->Init();
	}
	
	ICinemachineCameraInterface* OutGoingCamera = Cast<ICinemachineCameraInterface>(RootFrame->Source->CameraB);
	if (ActiveCamera != OutGoingCamera)
	{
		bool BackingOutOfBlend = false;
		float Duration = 0.0F;

		if (IsValid(ActiveCameraObject) && IsValid(RootFrame->Source->CameraB) && DeltaTime >= 0.0F)
		{
			if (LookupBlend.GetBlendTime() > UE_KINDA_SMALL_NUMBER)
			{
				BackingOutOfBlend = RootFrame->Source->CameraA == ActiveCameraObject;

				RootFrame->Source->CameraA = RootFrame->Source->CameraB;
				RootFrame->Source->BlendCurve = LookupBlend;
				Duration = LookupBlend.GetBlendTime();
			}
			RootFrame->Source->Duration = Duration;
			RootFrame->Source->TimeInBlend = 0.0F;
		}
		RootFrame->Source->CameraB = ActiveCameraObject;

		ICinemachineCameraInterface* CameraA = nullptr;
		if (RootFrame->Blend->IsComplete())
		{
			CameraA = RootFrame->GetSnapshotIfAppropriate(Cast<ICinemachineCameraInterface>(RootFrame->Source->CameraB), 0.0F);
		}
		else
		{
			bool bSnapshot = (RootFrame->Blend->GetState().BlendHint & static_cast<int32>(ECVBlendHintValue::FreezeWhenBlendingOut)) != 0;
			
			bool bBlendInheritPosition = IsValid(ActiveCameraObject) && (ActiveCamera->GetState().BlendHint & static_cast<int32>(ECVBlendHintValue::InheritPosition)) != 0;
			if (!bSnapshot && bBlendInheritPosition && RootFrame->Blend->Uses(ActiveCamera))
			{
				bSnapshot = true;
			}

			EObjectFlags Flags = RF_Public;
			Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
			if (HasAllFlags(RF_Transient))
			{
				Flags |= RF_Transient;
			}

			UCinemachineNestedBlendSource* NestedBlendSource = Cast<UCinemachineNestedBlendSource>(RootFrame->Blend->CameraA);
			bool bValidNestedBlendSource = IsValid(NestedBlendSource) && IsValid(NestedBlendSource->Blend);
			if (!bSnapshot && bValidNestedBlendSource)
			{
				if (UCinemachineNestedBlendSource* NestedBlendSource2 = Cast<UCinemachineNestedBlendSource>(NestedBlendSource->Blend->CameraA))
				{
					if (IsValid(NestedBlendSource2->Blend))
					{
						UCVSnapshotBlendSource* NewBlendSource = NewObject<UCVSnapshotBlendSource>(this, NAME_None, Flags);
						NewBlendSource->TakeSnapshot(Cast<ICinemachineCameraInterface>(NestedBlendSource2->Blend->CameraA));
						NestedBlendSource2->Blend->CameraA = NewBlendSource;
					}
				}
			}

			if (BackingOutOfBlend)
			{
				bSnapshot = true;
				Duration = RootFrame->Blend->TimeInBlend;
				if (bValidNestedBlendSource)
				{
					Duration += NestedBlendSource->Blend->Duration - NestedBlendSource->Blend->TimeInBlend;
				}
				else if (UCVSnapshotBlendSource* SnapshotBlendSource = Cast<UCVSnapshotBlendSource>(RootFrame->Blend->CameraA))
				{
					Duration += SnapshotBlendSource->RemainingTimeInBlend;
				}
			}

			if (bSnapshot)
			{
				UCVSnapshotBlendSource* NewBlendSource = NewObject<UCVSnapshotBlendSource>(this, NAME_None, Flags);
				NewBlendSource->TakeSnapshot(RootFrame);
				NewBlendSource->RemainingTimeInBlend = RootFrame->Blend->Duration - RootFrame->Blend->TimeInBlend;
				CameraA = NewBlendSource;
			}
			else
			{
				UCinemachineBlend* CopyBlend = NewObject<UCinemachineBlend>(this, NAME_None, Flags);
				CopyBlend->CopyFrom(RootFrame->Blend);

				UCinemachineNestedBlendSource* NewBlendSource = NewObject<UCinemachineNestedBlendSource>(this, NAME_None, Flags);
				NewBlendSource->Blend = CopyBlend;
				CameraA = NewBlendSource;
			}
		}

		RootFrame->Blend->CameraA = Cast<UObject>(CameraA);
		RootFrame->Blend->CameraB = ActiveCameraObject;
		RootFrame->Blend->BlendCurve = RootFrame->Source->BlendCurve;
		RootFrame->Blend->Duration = Duration;
		RootFrame->Blend->TimeInBlend = 0.0F;

		if (Duration > 0.0F)
		{
			if (UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld()))
			{
				FCVBlendEventParams Params;
				Params.Origin.SetObject(Cast<UObject>(Context));
				Params.Origin.SetInterface(Context);
				Params.Blend = RootFrame->Blend;
				Subsystem->OnBlendCreatedEvent.Broadcast(Params);
			}
		}
		RootFrame->Blend->CameraA = Cast<UObject>(CameraA);
		RootFrame->Blend->CameraB = ActiveCameraObject;
	}

	if (AdvanceBlend(RootFrame->Blend, DeltaTime))
	{
		RootFrame->Source->ClearBlend();
	}
	RootFrame->UpdateCameraState(Up, DeltaTime);
}

void UCinemachineCameraBlendStack::ProcessOverrideFrames(UCinemachineBlend* OutputBlend, int32 NumTopLayersToExclude)
{
	if (FrameStack.IsEmpty())
	{
		FrameStack.Add(CreateBlendStackFrame(this));
	}

	int32 LastActive = 0;
	int32 TopLayer = FMath::Max(0, FrameStack.Num() - NumTopLayersToExclude);
	for (int32 i = 1; i < TopLayer; ++i)
	{
		UCVBlendStackFrame* Frame = FrameStack[i];
		bool bIsValid = IsValid(Frame) && IsValid(Frame->Blend) && Frame->IsActive();
		if (bIsValid)
		{
			Frame->Blend->CopyFrom(Frame->Source);
			if (IsValid(Frame->Source->CameraA))
			{
				Frame->Blend->CameraA = Cast<UObject>(Frame->GetSnapshotIfAppropriate(Cast<ICinemachineCameraInterface>(Frame->Source->CameraA), Frame->Source->TimeInBlend));
			}
			
			if (!Frame->Blend->IsComplete())
			{
				if (!IsValid(Frame->Blend->CameraA))
				{
					Frame->Blend->CameraA = FrameStack.IsValidIndex(LastActive) ? Cast<UObject>(Frame->GetSnapshotIfAppropriate(FrameStack[LastActive], Frame->Blend->TimeInBlend)) : nullptr;
				}
				if (!IsValid(Frame->Blend->CameraB))
				{
					Frame->Blend->CameraB = FrameStack.IsValidIndex(LastActive) ? FrameStack[LastActive] : nullptr;
				}
			}
			LastActive = i;
		}
	}
	if (FrameStack.IsValidIndex(LastActive))
	{
		OutputBlend->CopyFrom(FrameStack[LastActive]->Blend);
	}
}

void UCinemachineCameraBlendStack::SetRootBlend(UCinemachineBlend* Blend)
{
	if (IsInitialized())
	{
		if (!IsValid(Blend))
		{
			FrameStack[0]->Blend->Duration = 0.0F;
		}
		else
		{
			FrameStack[0]->Blend->BlendCurve = Blend->BlendCurve;
			FrameStack[0]->Blend->TimeInBlend = Blend->TimeInBlend;
			FrameStack[0]->Blend->Duration = Blend->Duration;
		}
	}
}

void UCinemachineCameraBlendStack::OnEnable()
{
	FrameStack.Empty();
	FrameStack.Add(CreateBlendStackFrame(this));
}

void UCinemachineCameraBlendStack::OnDisable()
{
	FrameStack.Empty();
	NextFrameId = INDEX_NONE;
}
