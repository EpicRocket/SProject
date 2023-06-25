
#include "CinemachineFreeLook.h"
#include "Components/InputComponent.h"
#include "Camera/CinemachineInputAxisProviderInterface.h"
#include "Camera/CinemachineCoreSubSystem.h"
#include "Camera/VirtualCamera/CinemachineVirtualCameraComponent.h"
#include "Camera/Component/CinemachineComposerComponent.h"
#include "Shared/VectorExtension.h"

static void ComputeSmoothControlPoints(TArray<FVector4>& Knots, TArray<FVector4>& ControlPoints1, TArray<FVector4>& ControlPoints2)
{
	int32 NumPoints = Knots.Num();
	if (NumPoints <= 2)
	{
		if (NumPoints == 2)
		{
			ControlPoints1[0] = FMath::Lerp(Knots[0], Knots[1], 0.33333f);
			ControlPoints2[0] = FMath::Lerp(Knots[0], Knots[1], 0.66666f);
		}
		else if (NumPoints == 1)
			ControlPoints1[0] = ControlPoints2[0] = Knots[0];
		return;
	}

	TArray<float> A, B, C, R;
	A.SetNum(NumPoints);
	B.SetNum(NumPoints);
	C.SetNum(NumPoints);
	R.SetNum(NumPoints);
	for (int32 Axis = 0; Axis < 4; ++Axis)
	{
		int32 N = NumPoints - 1;

		// Linear into the first segment
		A[0] = 0;
		B[0] = 2;
		C[0] = 1;
		R[0] = Knots[0][Axis] + 2 * Knots[1][Axis];

		// Internal segments
		for (int32 i = 1; i < N - 1; ++i)
		{
			A[i] = 1;
			B[i] = 4;
			C[i] = 1;
			R[i] = 4 * Knots[i][Axis] + 2 * Knots[i + 1][Axis];
		}

		// Linear out of the last segment
		A[N - 1] = 2;
		B[N - 1] = 7;
		C[N - 1] = 0;
		R[N - 1] = 8 * Knots[N - 1][Axis] + Knots[N][Axis];

		// Solve with Thomas algorithm
		for (int32 i = 1; i < N; ++i)
		{
			float M = A[i] / B[i - 1];
			B[i] = B[i] - M * C[i - 1];
			R[i] = R[i] - M * R[i - 1];
		}

		// Compute ControlPoints1
		ControlPoints1[N - 1][Axis] = R[N - 1] / B[N - 1];
		for (int32 i = N - 2; i >= 0; --i)
			ControlPoints1[i][Axis] = (R[i] - C[i] * ControlPoints1[i + 1][Axis]) / B[i];

		// Compute ControlPoints2 from ControlPoints1
		for (int32 i = 0; i < N; i++)
			ControlPoints2[i][Axis] = 2 * Knots[i + 1][Axis] - ControlPoints1[i + 1][Axis];
		ControlPoints2[N - 1][Axis] = 0.5f * (Knots[N][Axis] + ControlPoints1[N - 1][Axis]);
	}
}

UCinemachineFreeLook::UCinemachineFreeLook()
	: bCommonLens(true)
	, YAxis(0.0F, 1.0F, false, true, 2.0F, 0.2F, 0.1F)
	, YAxisRecentering(false, 1.0F, 2.0F)
	, XAxis(-180.0F, 180.0F, true, false, 300.0F, 0.1F, 0.1F)
	, Heading(ECinemachineOrbitalTransposerHeadingDefinition::TargetForward, 4, 0)
	, RecenterToTargetHeading(false, 1.0F, 2.0F)
	, BindingMode(ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
	, SplineCurvature(0.2F)
{
	Orbits.Empty();
	Orbits.Reserve(UCinemachineFreeLook::RIG_COUNT);
	Orbits.Add(FCinemachineFreeLockOrbit(450.0F, 175.0F));
	Orbits.Add(FCinemachineFreeLockOrbit(250.0F, 300.0F));
	Orbits.Add(FCinemachineFreeLockOrbit(40.0F, 130.0F));

	Rigs.Reserve(UCinemachineFreeLook::RIG_COUNT);
	Orbitals.Reserve(UCinemachineFreeLook::RIG_COUNT);

	CachedOrbits.Empty();
	CachedOrbits.Reserve(UCinemachineFreeLook::RIG_COUNT);
	CachedOrbits.Add(FCinemachineFreeLockOrbit());
	CachedOrbits.Add(FCinemachineFreeLockOrbit());
	CachedOrbits.Add(FCinemachineFreeLockOrbit());

	CachedKnots.Reserve(5);
	CachedCtrl1.Reserve(5);
	CachedCtrl2.Reserve(5);
	for (int32 i = 0; i < 5; ++i)
	{
		CachedKnots.Add(FVector4{ 0.0F,0.0F,0.0F,0.0F });
		CachedCtrl1.Add(FVector4{ 0.0F,0.0F,0.0F,0.0F });
		CachedCtrl2.Add(FVector4{ 0.0F,0.0F,0.0F,0.0F });
	}
}

void UCinemachineFreeLook::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bDestroyed = true;
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UCinemachineFreeLook::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	YAxis.Validate();
	XAxis.Validate();
	RecenterToTargetHeading.Validate();
	YAxisRecentering.Validate();
	CommonLens.Validate();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

bool UCinemachineFreeLook::IsLiveChild(ICinemachineCameraInterface* InCamera, bool DominantChildOnly) const
{
	if (!RigsAreCreated())
	{
		return false;
	}

	float Y = GetYAxisValue();
	if (DominantChildOnly)
	{
		if (InCamera == Rigs[0])
		{
			return Y > 0.666F;
		}
		else if (InCamera == Rigs[1])
		{
			return Y < 0.333F;
		}
		else if (InCamera == Rigs[2])
		{
			return Y >= 0.333 && Y <= 0.666F;
		}
		return false;
	}
	if (InCamera == Rigs[1])
	{
		return true;
	}
	if (Y < 0.5F)
	{
		return InCamera == Rigs[2];
	}
	return InCamera == Rigs[0];
}

void UCinemachineFreeLook::OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta)
{
	UpdateRigCache();
	if (RigsAreCreated())
	{
		for (UCinemachineVirtualCameraComponent* VCamera : Rigs)
		{
			if (IsValid(VCamera))
			{
				VCamera->OnTargetObjectWarped(Target, LocationDelta);
			}
		}
	}
	Super::OnTargetObjectWarped(Target, LocationDelta);
}

void UCinemachineFreeLook::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	FVector Up = VCameraState.ReferenceUp;
	YAxis.Value = GetYAxisClosestValue(Location, Up);

	SetPreviousStateIsValid(true);
	SetWorldLocationAndRotation(Location, Rotation);
	VCameraState.RawLocation = Location;
	VCameraState.RawOrientation = Rotation;

	if (UpdateRigCache())
	{
		for (UCinemachineVirtualCameraComponent* VCamera : Rigs)
		{
			if (IsValid(VCamera))
			{
				VCamera->ForceCameraLocation(Location, Rotation);
			}
		}

		if (BindingMode != ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
		{
			XAxis.Value = Orbitals[1]->XAxis.Value;
		}

		PushSettingsToRigs();
		InternalUpdateCameraState(Up, -1.0F);
	}

	Super::ForceCameraLocation(Location, Rotation);
}

void UCinemachineFreeLook::InternalUpdateCameraState(FVector WorldUp, float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	UpdateTargetCache();
	UpdateRigCache();
	if (!RigsAreCreated())
	{
		return;
	}

	VCameraState = CalculateNewState(WorldUp, DeltaTime);
	ApplyLocationBlendMethod(VCameraState, TransitionParameters.BlendHint);

	if (IsValid(GetFollow()))
	{
		FVector LocationDelta = VCameraState.RawLocation - GetComponentLocation();
		SetWorldLocation(VCameraState.RawLocation);
		for (UCinemachineVirtualCameraComponent* VCamera : Rigs)
		{
			if (IsValid(VCamera))
			{
				VCamera->SetWorldLocation(VCamera->GetComponentLocation() - LocationDelta);
			}
		}
	}

	InvokePostPipelineStageCallback(this, ECinemachineStage::Finalize, VCameraState, DeltaTime);
	SetPreviousStateIsValid(true);

	bool ActiveCamera = GetPreviousStateIsValid() && World->GetSubsystem<UCinemachineCoreSubSystem>()->IsLive(this);
	if (ActiveCamera && DeltaTime >= 0.0F)
	{
		if (YAxis.Update(this, DeltaTime))
		{
			YAxisRecentering.CancelRecentering(this);
		}
	}
	PushSettingsToRigs();
	if (BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
	{
		XAxis.Value = 0.0F;
	}
}

void UCinemachineFreeLook::OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime)
{
	Super::OnTransitionFromCamera(FromCamera, WorldUp, DeltaTime);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (!RigsAreCreated())
	{
		return;
	}

	InvokeOnTransitionInExtensions(Cast<UCinemachineVirtualCameraBaseComponent>(FromCamera), WorldUp, DeltaTime);

	if (FromCamera != nullptr && TransitionParameters.bInheritLocation && !World->GetSubsystem<UCinemachineCoreSubSystem>()->IsLiveInBlend(this))
	{
		FVector CameraLocation = FromCamera->GetState().RawLocation;

		if (UCinemachineFreeLook* FreeLockCamera = Cast<UCinemachineFreeLook>(FromCamera))
		{
			UCinemachineOrbitalTransposerComponent* Orbital = FreeLockCamera->Orbitals.IsValidIndex(1) ? FreeLockCamera->Orbitals[1] : nullptr;
			if (IsValid(Orbital))
			{
				CameraLocation = Orbital->GetTargetCameraLocation(WorldUp);
			}
		}
		ForceCameraLocation(CameraLocation, FromCamera->GetState().FinalOrientation());
	}
	UpdateCameraState(WorldUp, DeltaTime);
	TransitionParameters.CameraActivatedEvent.Broadcast(this, Cast<UCinemachineVirtualCameraBaseComponent>(FromCamera));
}

bool UCinemachineFreeLook::RequiresUserInput()
{
	return true;
}

void UCinemachineFreeLook::SetPreviousStateIsValid(bool bValue)
{
	if (false == bValue)
	{
		for (UCinemachineVirtualCameraComponent* VCamera : Rigs)
		{
			if (IsValid(VCamera))
			{
				VCamera->SetPreviousStateIsValid(false);
			}
		}
	}
	Super::SetPreviousStateIsValid(bValue);
}

void UCinemachineFreeLook::SetInputAxisProvider(TScriptInterface<ICinemachineInputAxisProviderInterface> InsertProvider)
{
	Super::SetInputAxisProvider(InsertProvider);
	XAxis.SetInputAxisProvider(EAxis::X, InputAxisProvider);
	YAxis.SetInputAxisProvider(EAxis::Y, InputAxisProvider);
	for (UCinemachineOrbitalTransposerComponent* Orbital : Orbitals)
	{
		if (IsValid(Orbital))
		{
			Orbital->UpdateInputAxisProvider();
		}
	}
}

UCinemachineVirtualCameraComponent* UCinemachineFreeLook::GetRig(int32 Index)
{
	return UpdateRigCache() && Rigs.IsValidIndex(Index) ? Rigs[Index] : nullptr;
}

FVector UCinemachineFreeLook::GetLocalLocationForCameraFromInput(float Alpha)
{
	if (!RigsAreCreated())
	{
		return FVector::ZeroVector;
	}

	UpdateCachedSpline();
	int32 N = 1;
	if (Alpha > 0.5F)
	{
		Alpha -= 0.5F;
		N = 2;
	}

	FVector Result = UVectorExtension::Bezier3(CachedKnots[N], CachedCtrl1[N], CachedCtrl2[N], CachedKnots[N + 1], Alpha * 2.0F);
	return FVector(Result.Z, Result.X, Result.Y);
}

void UCinemachineFreeLook::OnEnable()
{
	bDestroyed = false;
	Super::OnEnable();

	for (UCinemachineVirtualCameraComponent* VCamera : Rigs)
	{
		if (IsValid(VCamera))
		{
			VCamera->SetEnable(true);
		}
	}
}

void UCinemachineFreeLook::OnDisable()
{
	for (UCinemachineVirtualCameraComponent* VCamera : Rigs)
	{
		if (IsValid(VCamera))
		{
			VCamera->SetEnable(false);
		}
	}
	Super::OnDisable();
}

float UCinemachineFreeLook::GetYAxisClosestValue(FVector CameraLocation, FVector Up)
{
	USceneComponent* Follow = GetFollow();
	if (IsValid(Follow))
	{
		FQuat BetweenRotation = FQuat::FindBetweenNormals(Follow->GetUpVector(), Up);
		FVector Direction = BetweenRotation * (CameraLocation - Follow->GetComponentLocation());
		FVector FlatDirection(Direction.X, Direction.Y, 0.0F);
		if (!FlatDirection.IsNearlyZero())
		{
			float Angle = UVectorExtension::SignedAngle(FlatDirection, FVector::BackwardVector, FVector::UpVector);
			Direction = FQuat(FVector::UpVector, FMath::DegreesToRadians(Angle)) * Direction;
		}
		Direction.Y = 0.0F;

		return SteepestDescent(Direction.GetSafeNormal() * (CameraLocation - Follow->GetComponentLocation()).SizeSquared());
	}
	return YAxis.Value;
}

float UCinemachineFreeLook::SteepestDescent(FVector CameraOffset)
{
	const int32 MAX_ITERATION = 10;
	const float EPSILON = 5.0E-4F;

	auto AngleFunction = [&](float Input) -> float
	{
		FVector Point = GetLocalLocationForCameraFromInput(Input);
		return FMath::Abs(UVectorExtension::SignedAngle(CameraOffset, Point, FVector::RightVector));
	};

	// approximating derivative using symmetric difference quotient (finite diff)
	auto SlopeOfAngleFunction = [&](float Input) -> float
	{
		float AngleBehind = AngleFunction(Input - EPSILON);
		float AngleAfter = AngleFunction(Input + EPSILON);
		return (AngleAfter - AngleBehind) / (2.0F * EPSILON);
	};

	// initial guess based on closest line (approximating spline) to point 
	auto InitialGuess = [&](FVector CameraLocationRigSpace) -> float
	{
		UpdateCachedSpline();
		FVector4 PB = CachedKnots[1];
		FVector4 PM = CachedKnots[2];
		FVector4 PT = CachedKnots[3];
		float T1 = UVectorExtension::ClosestPointOnSegment(CameraLocationRigSpace, PB, PM);
		float Dist1 = (FMath::Lerp(PB, PM, T1) - CameraLocationRigSpace).SizeSquared();
		float T2 = UVectorExtension::ClosestPointOnSegment(CameraLocationRigSpace, PM, PT);
		float Dist2 = (FMath::Lerp(PM, PT, T2) - CameraLocationRigSpace).SizeSquared();

		return Dist1 < Dist2 ? FMath::Lerp(0.0F, 0.5F, T1) : FMath::Lerp(0.5F, 1.0F, T2);
	};

	const int32 MaxIteration = 10;
	const float Epsilon = 0.00005F;
	float X = InitialGuess(CameraOffset);
	for (int32 i = 0; i < MaxIteration; ++i)
	{
		float Angle = AngleFunction(X);
		float Slope = SlopeOfAngleFunction(X);
		if (FMath::Abs(Slope) < Epsilon || FMath::Abs(Angle) < Epsilon)
		{
			break;
		}
		X = FMath::Clamp(X - (Angle / Slope), 0.0F, 1.0F);
	}
	return X;
}

bool UCinemachineFreeLook::UpdateRigCache()
{
	if (bDestroyed)
	{
		return false;
	}

	if (RigsAreCreated())
	{
		return true;
	}

	CachedXAxisHeading = XAxis.Value;
	LastHeadingUpdateFrame = 0;
	if (!RigsAreCreated())
	{
		Rigs.Empty();
		Orbitals.Empty();

		for (USceneComponent* Child : GetAttachChildren())
		{
			UCinemachineVirtualCameraComponent* CinemachineComponent = Cast<UCinemachineVirtualCameraComponent>(Child);
			if (!IsValid(CinemachineComponent))
			{
				continue;
			}
			if (Rigs.Num() == UCinemachineFreeLook::RIG_COUNT)
			{
				check(!TEXT("VCamera는 3개 설정 해야 합니다. 3개를 초과하였습니다."));
			}

			CinemachineComponent->UpdateComponentPipeline();
			CinemachineComponent->StandbyUpdateMode = StandbyUpdateMode;

			Rigs.Emplace(CinemachineComponent);
		}

		Rigs.Sort([](UCinemachineVirtualCameraComponent& A, UCinemachineVirtualCameraComponent& B) -> bool
		{
			return A.GetPriority() < B.GetPriority();
		});

		for (UCinemachineVirtualCameraComponent* Rig : Rigs)
		{
			UCinemachineOrbitalTransposerComponent* OrbitalTransposer = Rig->GetCinemachineComponent<UCinemachineOrbitalTransposerComponent>();
			if (!IsValid(OrbitalTransposer))
			{
				check(!TEXT("VCamera 하위에 CinemachineOrbitalTransposerComponent가 존재해야 합니다."));
			}

			OrbitalTransposer->bHeadingIsSlave = true;
			OrbitalTransposer->HeadingUpdater = [this](UCinemachineOrbitalTransposerComponent* OrbitalTransposer, float DeltaTime, FVector Up) -> float
			{
				return UpdateXAxisHeading(OrbitalTransposer, DeltaTime, Up);
			};
			OrbitalTransposer->RecenterToTargetHeading.bEnabled = false;

			Orbitals.Emplace(OrbitalTransposer);
		}

		if (!RigsAreCreated())
		{
			return false;
		}

		BlendA = NewObject<UCinemachineBlend>(this, TEXT("CinemachineFreeLook BlendA"), RF_Transient);
		BlendA->CameraA = Rigs[1];
		BlendA->CameraB = Rigs[0];
		BlendA->BlendCurve = BlendCurve;
		BlendA->TimeInBlend = 0.0F;
		BlendA->Duration = 1.0F;

		BlendB = NewObject<UCinemachineBlend>(this, TEXT("CinemachineFreeLook BlendB"), RF_Transient);
		BlendB->CameraA = Rigs[2];
		BlendB->CameraB = Rigs[1];
		BlendA->BlendCurve = BlendCurve;
		BlendA->TimeInBlend = 0.0F;
		BlendA->Duration = 1.0F;
	}

	return RigsAreCreated();
}

float UCinemachineFreeLook::UpdateXAxisHeading(UCinemachineOrbitalTransposerComponent* Orbital, float DeltaTime, FVector Up)
{
	UWorld* World = GetWorld();

	if (bDestroyed || !IsValid(World))
	{
		return 0.0F;
	}

	if (!IsValid(Orbital))
	{
		return CachedXAxisHeading;
	}

	if (!GetPreviousStateIsValid())
	{
		DeltaTime = -1.0F;
	}

	if (LastHeadingUpdateFrame != GFrameCounter || DeltaTime < 0.0F)
	{
		LastHeadingUpdateFrame = GFrameCounter;
		float OldValue = XAxis.Value;
		CachedXAxisHeading = Orbital->UpdateHeading(DeltaTime, Up, XAxis, RecenterToTargetHeading, World->GetSubsystem<UCinemachineCoreSubSystem>()->IsLive(this));

		if (BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
		{
			XAxis.Value = OldValue;
		}
	}

	return CachedXAxisHeading;
}

void UCinemachineFreeLook::PushSettingsToRigs()
{
	if (!RigsAreCreated())
	{
		return;
	}

	for (int32 i = 0; i < UCinemachineFreeLook::RIG_COUNT; ++i)
	{
		if (!IsValid(Rigs[i]))
		{
			UE_LOG(LogTemp, Warning, TEXT("Rig %d is invalid"), i);
			continue;
		}

		if (!IsValid(Orbitals[i]))
		{
			UE_LOG(LogTemp, Warning, TEXT("Orbital %d is invalid"), i);
			continue;
		}

		if (bCommonLens)
		{
			Rigs[i]->VCameraLens = CommonLens;
		}

		Rigs[i]->SetFollow(nullptr);
		Rigs[i]->StandbyUpdateMode = StandbyUpdateMode;
		Rigs[i]->FollowTargetAttachment = FollowTargetAttachment;
		Rigs[i]->LookAtTargetAttachment = LookAtTargetAttachment;

		if (!GetPreviousStateIsValid())
		{
			Rigs[i]->SetPreviousStateIsValid(false);
			Rigs[i]->SetWorldLocationAndRotation(GetComponentLocation(), GetComponentRotation());
		}

		Orbitals[i]->FollowOffset = GetLocalLocationForCameraFromInput(GetYAxisValue());
		Orbitals[i]->BindingMode = BindingMode;
		Orbitals[i]->Heading = Heading;
		Orbitals[i]->XAxis.Value = XAxis.Value;

		if (BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
		{
			Rigs[i]->SetStateRawLocation(VCameraState.RawLocation);
		}
	}
}

float UCinemachineFreeLook::GetYAxisValue() const
{
	float Range = YAxis.MaxValue - YAxis.MinValue;
	return Range > UE_KINDA_SMALL_NUMBER ? YAxis.Value / Range : 0.5F;
}

FCinemachineCameraState UCinemachineFreeLook::CalculateNewState(FVector WorldUp, float DeltaTime)
{
	YAxisRecentering.DoRecentering(this, YAxis, DeltaTime, 0.5F);
	float Alpha = GetYAxisValue();
	if (Alpha > 0.5F)
	{
		if (IsValid(BlendA))
		{
			BlendA->TimeInBlend = (Alpha - 0.5F) * 2.0F;
			BlendA->UpdateCameraState(WorldUp, DeltaTime);
			return BlendA->GetState();
		}
	}
	else
	{
		if (IsValid(BlendB))
		{
			BlendB->TimeInBlend = Alpha * 2.0F;
			BlendB->UpdateCameraState(WorldUp, DeltaTime);
			return BlendB->GetState();
		}
	}
	return PullStateFromVirtualCamera(WorldUp, VCameraLens);
}

void UCinemachineFreeLook::UpdateCachedSpline()
{
	bool bCacheIsValid = (CachedOrbits.Num() == UCinemachineFreeLook::RIG_COUNT) && FMath::IsNearlyEqual(CachedTension, SplineCurvature);
	for (int32 i = 0; i < UCinemachineFreeLook::RIG_COUNT && bCacheIsValid; ++i)
	{
		bCacheIsValid = FMath::IsNearlyEqual(CachedOrbits[i].Height, Orbits[i].Height) && FMath::IsNearlyEqual(CachedOrbits[i].Radius, Orbits[i].Radius);
	}

	static const FVector4 ZeroVector4 = FVector4::Zero();

	if (!bCacheIsValid)
	{
		float Alpha = SplineCurvature;
		CachedKnots[1] = FVector4(0.0F, Orbits[2].Height, -Orbits[2].Radius, 0.0F);
		CachedKnots[2] = FVector4(0.0F, Orbits[1].Height, -Orbits[1].Radius, 0.0F);
		CachedKnots[3] = FVector4(0.0F, Orbits[0].Height, -Orbits[0].Radius, 0.0F);
		CachedKnots[0] = FMath::Lerp(CachedKnots[1], ZeroVector4, Alpha);
		CachedKnots[4] = FMath::Lerp(CachedKnots[3], ZeroVector4, Alpha);

		ComputeSmoothControlPoints(CachedKnots, CachedCtrl1, CachedCtrl2);
		
		for (int32 i = 0; i < UCinemachineFreeLook::RIG_COUNT; ++i)
		{
			CachedOrbits[i] = Orbits[i];
		}
		CachedTension = SplineCurvature;
	}
}
