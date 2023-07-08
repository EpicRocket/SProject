

#include "CinemachineFreeLookComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CinemachineInputAxisProviderInterface.h"
#include "Camera/CinemachineCoreSubSystem.h"
#include "Camera/VirtualCamera/CinemachineVirtualCameraComponent.h"
#include "Camera/Component/CinemachineComposer.h"
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

UCinemachineFreeLookComponent::UCinemachineFreeLookComponent()
	: bCommonLens(true)
	, YAxis(0.0F, 1.0F, false, true, 2.0F, 0.2F, 0.1F)
	, YAxisRecentering(false, 1.0F, 2.0F)
	, XAxis(-180.0F, 180.0F, true, false, 300.0F, 0.1F, 0.1F)
	, Heading(ECVOrbitalTransposerHeadingDef::TargetForward, 4, 0)
	, RecenterToTargetHeading(false, 1.0F, 2.0F)
	, BindingMode(ECVBindingMode::SimpleFollowWithWorldUp)
	, SplineCurvature(0.2F)
{
	Orbits.Empty();
	Orbits.Reserve(UCinemachineFreeLookComponent::RIG_COUNT);
	Orbits.Emplace(ECVFreeLookRigType::TopRig, FCinemachineFreeLockOrbit(450.0F, 175.0F));
	Orbits.Emplace(ECVFreeLookRigType::MiddleRig, FCinemachineFreeLockOrbit(250.0F, 300.0F));
	Orbits.Emplace(ECVFreeLookRigType::BottomRig, FCinemachineFreeLockOrbit(40.0F, 130.0F));

	Rigs.Reserve(UCinemachineFreeLookComponent::RIG_COUNT);
	Orbitals.Reserve(UCinemachineFreeLookComponent::RIG_COUNT);

	CachedOrbits.Empty();
	CachedOrbits.Reserve(UCinemachineFreeLookComponent::RIG_COUNT);
	CachedOrbits.Emplace(ECVFreeLookRigType::TopRig, FCinemachineFreeLockOrbit());
	CachedOrbits.Emplace(ECVFreeLookRigType::MiddleRig, FCinemachineFreeLockOrbit());
	CachedOrbits.Emplace(ECVFreeLookRigType::BottomRig, FCinemachineFreeLockOrbit());

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

void UCinemachineFreeLookComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bDestroyed = true;
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UCinemachineFreeLookComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_PROPERTY(PropertyChangedEvent, UCinemachineFreeLookComponent, TopRigAimStageClass, TopRigAimStageTemplate, ECVStage::Aim);
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_PROPERTY(PropertyChangedEvent, UCinemachineFreeLookComponent, MiddleRigAimStageClass, MiddleRigAimStageTemplate, ECVStage::Aim);
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_PROPERTY(PropertyChangedEvent, UCinemachineFreeLookComponent, BottomRigAimStageClass, BottomRigAimStageTemplate, ECVStage::Aim);
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
void UCinemachineFreeLookComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_CHAIN_PROPERTY(PropertyChangedEvent, UCinemachineFreeLookComponent, TopRigAimStageClass, TopRigAimStageTemplate, ECVStage::Aim);
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_CHAIN_PROPERTY(PropertyChangedEvent, UCinemachineFreeLookComponent, MiddleRigAimStageClass, MiddleRigAimStageTemplate, ECVStage::Aim);
	CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_CHAIN_PROPERTY(PropertyChangedEvent, UCinemachineFreeLookComponent, BottomRigAimStageClass, BottomRigAimStageTemplate, ECVStage::Aim);
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
#endif

bool UCinemachineFreeLookComponent::IsLiveChild(ICinemachineCameraInterface* InCamera, bool DominantChildOnly) const
{
	if (!IsInitialized())
	{
		return false;
	}

	if (Rigs.Num() != UCinemachineFreeLookComponent::RIG_COUNT)
	{
		return false;
	}

	float Y = GetYAxisValue();
	if (DominantChildOnly)
	{
		if (InCamera == Rigs[ECVFreeLookRigType::TopRig])
		{
			return Y > 0.666F;
		}
		else if (InCamera == Rigs[ECVFreeLookRigType::MiddleRig])
		{
			return Y < 0.333F;
		}
		else if (InCamera == Rigs[ECVFreeLookRigType::BottomRig])
		{
			return Y >= 0.333 && Y <= 0.666F;
		}
		return false;
	}
	if (InCamera == Rigs[ECVFreeLookRigType::MiddleRig])
	{
		return true;
	}
	if (Y < 0.5F)
	{
		return InCamera == Rigs[ECVFreeLookRigType::BottomRig];
	}
	return InCamera == Rigs[ECVFreeLookRigType::TopRig];
}

void UCinemachineFreeLookComponent::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	if (!IsInitialized())
	{
		return;
	}

	FVector Up = VCameraState.ReferenceUp;
	YAxis.Value = GetYAxisClosestValue(Location, Up);

	SetPreviousStateIsValid(true);
	SetWorldLocationAndRotation(Location, Rotation);
	VCameraState.RawLocation = Location;
	VCameraState.RawOrientation = Rotation;

	for (auto& [Type, Rig] : Rigs)
	{
		if (IsValid(Rig))
		{
			Rig->ForceCameraLocation(Location, Rotation);
		}
	}

	if (BindingMode != ECVBindingMode::SimpleFollowWithWorldUp)
	{
		XAxis.Value = Orbitals[ECVFreeLookRigType::MiddleRig]->OrbitalTransposerData.XAxis.Value;
	}

	PushSettingsToRigs();
	InternalUpdateCameraState(Up, -1.0F);

	Super::ForceCameraLocation(Location, Rotation);
}

void UCinemachineFreeLookComponent::InternalUpdateCameraState(FVector WorldUp, float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	UpdateTargetCache();
#if WITH_EDITOR
	if (World->IsGameWorld())
	{
		if (!YAxis.HasInputProvider() || !XAxis.HasInputProvider())
		{
			UpdateInputAxisProvider();
		}
	}
#endif

	VCameraState = CalculateNewState(WorldUp, DeltaTime);
	ApplyLocationBlendMethod(VCameraState, TransitionParameters.BlendHint);

	if (IsValid(GetFollow()))
	{
		// TODO: 한번 주석처리 하고 실행해보자
		FVector LocationDelta = VCameraState.RawLocation - GetComponentLocation();
		SetWorldLocation(VCameraState.RawLocation);
		for (auto& [Type, Rig] : Rigs)
		{
			if (IsValid(Rig))
			{
				Rig->SetWorldLocation(Rig->GetComponentLocation() - LocationDelta);
			}
		}
	}

	InvokePostPipelineStageCallback(this, ECVStage::Finalize, VCameraState, DeltaTime);
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
	if (BindingMode == ECVBindingMode::SimpleFollowWithWorldUp)
	{
		XAxis.Value = 0.0F;
	}
}

void UCinemachineFreeLookComponent::OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime)
{
	Super::OnTransitionFromCamera(FromCamera, WorldUp, DeltaTime);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	InvokeOnTransitionInExtensions(Cast<UCinemachineVirtualCameraBaseComponent>(FromCamera), WorldUp, DeltaTime);

	if (FromCamera != nullptr && TransitionParameters.bInheritLocation && !World->GetSubsystem<UCinemachineCoreSubSystem>()->IsLiveInBlend(this))
	{
		FVector CameraLocation = FromCamera->GetState().RawLocation;

		UCinemachineFreeLookComponent* FreeLockCamera = Cast<UCinemachineFreeLookComponent>(FromCamera);
		if (IsValid(FreeLockCamera) && FreeLockCamera->IsInitialized())
		{
			UCinemachineOrbitalTransposer* Orbital = FreeLockCamera->Orbitals[ECVFreeLookRigType::MiddleRig];
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

bool UCinemachineFreeLookComponent::RequiresUserInput()
{
	return true;
}

void UCinemachineFreeLookComponent::SetPreviousStateIsValid(bool bValue)
{
	if (false == bValue)
	{
		for (auto& [Type, Rig] : Rigs)
		{
			if (IsValid(Rig))
			{
				Rig->SetPreviousStateIsValid(false);
			}
		}
	}
	Super::SetPreviousStateIsValid(bValue);
}

void UCinemachineFreeLookComponent::UpdateInputAxisProvider()
{
	ICinemachineInputAxisProviderInterface* Provider = Cast<ICinemachineInputAxisProviderInterface>(InputAxisProvider);
	XAxis.SetInputAxisProvider(EAxis::X, Provider);
	YAxis.SetInputAxisProvider(EAxis::Y, Provider);
	for (auto& [Type, Orbital] : Orbitals)
	{
		if (IsValid(Orbital))
		{
			Orbital->UpdateInputAxisProvider();
		}
	}
}

UCinemachineVirtualCameraComponent* UCinemachineFreeLookComponent::GetRig(ECVFreeLookRigType RigType)
{
	return IsInitialized() ? Rigs[RigType] : nullptr;
}

FVector UCinemachineFreeLookComponent::GetLocalLocationForCameraFromInput(float Alpha)
{
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

void UCinemachineFreeLookComponent::OnInitailize_Implementation()
{
	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (HasAllFlags(RF_Transient))
	{
		Flags |= RF_Transient;
	}

	UCinemachineVirtualCameraComponent* NewTemplate = NewObject<UCinemachineVirtualCameraComponent>(this, UCinemachineVirtualCameraComponent::StaticClass(), NAME_None, RF_ArchetypeObject | RF_Transactional | RF_Public, nullptr);
	check(NewTemplate);

	NewTemplate->SetStageTemplate(NewTemplate->BodyStageClass, UCinemachineOrbitalTransposer::StaticClass(), NewTemplate->BodyStageTemplate, ECVStage::Body);
	if (UCinemachineOrbitalTransposer* BodyStage = Cast<UCinemachineOrbitalTransposer>(NewTemplate->BodyStageTemplate))
	{
		BodyStage->TransposerData.XDamping = BottomRigTransposerDamp.X;
		BodyStage->TransposerData.YDamping = BottomRigTransposerDamp.Y;
		BodyStage->TransposerData.ZDamping = BottomRigTransposerDamp.Z;
	}

	UCinemachineVirtualCameraComponent* NewRig = nullptr;

	NewTemplate->AimStageClass = TopRigAimStageClass;
	NewTemplate->AimStageTemplate = TopRigAimStageTemplate;
	NewRig = NewObject<UCinemachineVirtualCameraComponent>(this, UCinemachineVirtualCameraComponent::StaticClass(), TEXT("TopRig(CV)"), Flags, NewTemplate);
	NewRig->SetupAttachment(this);
	NewRig->RegisterComponent();
	Rigs.Emplace(ECVFreeLookRigType::TopRig, NewRig);
	Orbitals.Emplace(ECVFreeLookRigType::TopRig, NewRig->GetCinemachineStage<UCinemachineOrbitalTransposer>());
	Rigs[ECVFreeLookRigType::TopRig]->Init();

	NewTemplate->AimStageClass = MiddleRigAimStageClass;
	NewTemplate->AimStageTemplate = MiddleRigAimStageTemplate;
	NewRig = NewObject<UCinemachineVirtualCameraComponent>(this, UCinemachineVirtualCameraComponent::StaticClass(), TEXT("MiddleRig(CV)"), Flags, NewTemplate);
	NewRig->SetupAttachment(this);
	NewRig->RegisterComponent();
	Rigs.Emplace(ECVFreeLookRigType::MiddleRig, NewRig);
	Orbitals.Emplace(ECVFreeLookRigType::MiddleRig, NewRig->GetCinemachineStage<UCinemachineOrbitalTransposer>());
	Rigs[ECVFreeLookRigType::MiddleRig]->Init();

	NewTemplate->AimStageClass = BottomRigAimStageClass;
	NewTemplate->AimStageTemplate = BottomRigAimStageTemplate;
	NewRig = NewObject<UCinemachineVirtualCameraComponent>(this, UCinemachineVirtualCameraComponent::StaticClass(), TEXT("BottomRig(CV)"), Flags, NewTemplate);
	NewRig->SetupAttachment(this);
	NewRig->RegisterComponent();
	Rigs.Emplace(ECVFreeLookRigType::BottomRig, NewRig);
	Orbitals.Emplace(ECVFreeLookRigType::BottomRig, NewRig->GetCinemachineStage<UCinemachineOrbitalTransposer>());
	Rigs[ECVFreeLookRigType::BottomRig]->Init();
	
	if (IsEnable())
	{
		for (auto& [Type, Rig] : Rigs)
		{
			Rig->SetEnable(true);
		}
	}

	CachedXAxisHeading = XAxis.Value;
	LastHeadingUpdateFrame = 0;

	for (auto& [Type, Rig] : Rigs)
	{
		Rig->StandbyUpdateMode = StandbyUpdateMode;
	}

	for (auto& [Type, Orbital] : Orbitals)
	{
		Orbital->bHeadingIsSlave = true;
		Orbital->HeadingUpdater = [this](UCinemachineOrbitalTransposer* OrbitalTransposer, float DeltaTime, FVector Up) -> float
		{
			return UpdateXAxisHeading(OrbitalTransposer, DeltaTime, Up);
		};
		Orbital->OrbitalTransposerData.RecenterToTargetHeading.bEnabled = false;
	}

	BlendA = NewObject<UCinemachineBlend>(this, TEXT("CinemachineFreeLook BlendA"), RF_Transient);
	BlendA->CameraA = Rigs[ECVFreeLookRigType::MiddleRig];
	BlendA->CameraB = Rigs[ECVFreeLookRigType::TopRig];
	BlendA->BlendCurve = BlendCurve;
	BlendA->TimeInBlend = 0.0F;
	BlendA->Duration = 1.0F;

	BlendB = NewObject<UCinemachineBlend>(this, TEXT("CinemachineFreeLook BlendB"), RF_Transient);
	BlendB->CameraA = Rigs[ECVFreeLookRigType::BottomRig];
	BlendB->CameraB = Rigs[ECVFreeLookRigType::MiddleRig];
	BlendB->BlendCurve = BlendCurve;
	BlendB->TimeInBlend = 0.0F;
	BlendB->Duration = 1.0F;

	UpdateInputAxisProvider();
}

void UCinemachineFreeLookComponent::OnEnable()
{
	bDestroyed = false;
	Super::OnEnable();

	YAxis.InputAxisValue = 0.0F;
	XAxis.InputAxisValue = 0.0F;

	for (auto& [Type, Rig] : Rigs)
	{
		if (IsValid(Rig))
		{
			Rig->SetEnable(true);
		}
	}
}

void UCinemachineFreeLookComponent::OnDisable()
{
	for (auto& [Type, Rig] : Rigs)
	{
		if (IsValid(Rig))
		{
			Rig->SetEnable(false);
		}
	}
	Super::OnDisable();
}

void UCinemachineFreeLookComponent::SetRig(ECVFreeLookRigType RigType, UCinemachineVirtualCameraComponent* Rig)
{
	if (!IsValid(Rig))
	{
		return;
	}

	UCinemachineOrbitalTransposer* Orbital = Rig->GetCinemachineStage<UCinemachineOrbitalTransposer>();
	if (!IsValid(Orbital))
	{
		return;
	}

	if (Rigs.Contains(RigType))
	{
		Rigs[RigType] = Rig;
	}
	else
	{
		Rigs.Emplace(RigType, Rig);
	}

	SetOrbital(RigType, Orbital);
}

void UCinemachineFreeLookComponent::SetOrbital(ECVFreeLookRigType RigType, UCinemachineOrbitalTransposer* Orbital)
{
	if (Orbitals.Contains(RigType))
	{
		Orbitals[RigType] = Orbital;
	}
	else
	{
		Orbitals.Emplace(RigType, Orbital);
	}
}

float UCinemachineFreeLookComponent::GetYAxisClosestValue(FVector CameraLocation, FVector Up)
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

float UCinemachineFreeLookComponent::SteepestDescent(FVector CameraOffset)
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

float UCinemachineFreeLookComponent::UpdateXAxisHeading(UCinemachineOrbitalTransposer* Orbital, float DeltaTime, FVector Up)
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

		if (BindingMode == ECVBindingMode::SimpleFollowWithWorldUp)
		{
			XAxis.Value = OldValue;
		}
	}

	return CachedXAxisHeading;
}

void UCinemachineFreeLookComponent::PushSettingsToRigs()
{
	if (!IsInitialized())
	{
		return;
	}

	for (ECVFreeLookRigType RigType : { ECVFreeLookRigType::TopRig, ECVFreeLookRigType::MiddleRig, ECVFreeLookRigType::BottomRig })
	{
		if (!IsValid(Rigs[RigType]))
		{
			continue;
		}

		if (!IsValid(Orbitals[RigType]))
		{
			continue;
		}

		if (bCommonLens)
		{
			Rigs[RigType]->VCameraLens = CommonLens;
		}

		Rigs[RigType]->SetFollow(nullptr);
		Rigs[RigType]->StandbyUpdateMode = StandbyUpdateMode;
		Rigs[RigType]->FollowTargetAttachment = FollowTargetAttachment;
		Rigs[RigType]->LookAtTargetAttachment = LookAtTargetAttachment;

		if (!GetPreviousStateIsValid())
		{
			Rigs[RigType]->SetPreviousStateIsValid(false);
			Rigs[RigType]->SetWorldLocationAndRotation(GetComponentLocation(), GetComponentRotation());
		}

		Orbitals[RigType]->TransposerData.FollowOffset = GetLocalLocationForCameraFromInput(GetYAxisValue());
		Orbitals[RigType]->TransposerData.BindingMode = BindingMode;
		Orbitals[RigType]->OrbitalTransposerData.Heading = Heading;
		Orbitals[RigType]->OrbitalTransposerData.XAxis.Value = XAxis.Value;

		if (BindingMode == ECVBindingMode::SimpleFollowWithWorldUp)
		{
			Rigs[RigType]->SetStateRawLocation(VCameraState.RawLocation);
		}
	}
}

float UCinemachineFreeLookComponent::GetYAxisValue() const
{
	float Range = YAxis.MaxValue - YAxis.MinValue;
	return Range > UE_KINDA_SMALL_NUMBER ? YAxis.Value / Range : 0.5F;
}

FCinemachineCameraState UCinemachineFreeLookComponent::CalculateNewState(FVector WorldUp, float DeltaTime)
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

void UCinemachineFreeLookComponent::UpdateCachedSpline()
{
	bool bCacheIsValid = (CachedOrbits.Num() == UCinemachineFreeLookComponent::RIG_COUNT) && FMath::IsNearlyEqual(CachedTension, SplineCurvature);

	for (int32 i = 0; i < UCinemachineFreeLookComponent::RIG_COUNT && bCacheIsValid; ++i)
	{
		ECVFreeLookRigType RigType = static_cast<ECVFreeLookRigType>(i);
		bCacheIsValid = FMath::IsNearlyEqual(CachedOrbits[RigType].Height, Orbits[RigType].Height) && FMath::IsNearlyEqual(CachedOrbits[RigType].Radius, Orbits[RigType].Radius);
	}

	static const FVector4 ZeroVector4 = FVector4::Zero();

	if (!bCacheIsValid)
	{
		float Alpha = SplineCurvature;
		CachedKnots[1] = FVector4(0.0F, Orbits[ECVFreeLookRigType::BottomRig].Height, -Orbits[ECVFreeLookRigType::BottomRig].Radius, 0.0F);
		CachedKnots[2] = FVector4(0.0F, Orbits[ECVFreeLookRigType::MiddleRig].Height, -Orbits[ECVFreeLookRigType::MiddleRig].Radius, 0.0F);
		CachedKnots[3] = FVector4(0.0F, Orbits[ECVFreeLookRigType::TopRig].Height, -Orbits[ECVFreeLookRigType::TopRig].Radius, 0.0F);
		CachedKnots[0] = FMath::Lerp(CachedKnots[1], ZeroVector4, Alpha);
		CachedKnots[4] = FMath::Lerp(CachedKnots[3], ZeroVector4, Alpha);

		ComputeSmoothControlPoints(CachedKnots, CachedCtrl1, CachedCtrl2);
		
		for (ECVFreeLookRigType RigType : { ECVFreeLookRigType::TopRig, ECVFreeLookRigType::MiddleRig, ECVFreeLookRigType::BottomRig })
		{
			CachedOrbits[RigType] = Orbits[RigType];

		}
		CachedTension = SplineCurvature;
	}
}
