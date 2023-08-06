

#include "CinemachineOrbitalFollow.h"
#include "Shared/SplineHelper.h"
#include "Shared/VectorExtension.h"
#include "Camera/Core/CinemachineCoreSubSystem.h"
#include "Camera/Core/CinemachineVirtualCameraBaseComponent.h"

#define LOCTEXT_NAMESPACE "CinemachineOrbitalFollow"
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
	A.Init(0.0F, 5);
	B.Init(0.0F, 5);
	C.Init(0.0F, 5);
	R.Init(0.0F, 5);
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

static float MapTo01(float ValueToMap, float Min, float Max)
{
	return (ValueToMap - Min) / (Max - Min);
}
#undef LOCTEXT_NAMESPACE

//! FCVOrbitSplineCache

FCVOrbitSplineCache::FCVOrbitSplineCache()
{
	const FVector4 ZeroVector4 = FVector4(0.0F, 0.0F, 0.0F, 0.0F);
	CachedKnots.Init(ZeroVector4, 5);
	CachedCtrl1.Init(ZeroVector4, 5);
	CachedCtrl2.Init(ZeroVector4, 5);
}

void FCVOrbitSplineCache::UpdateOrbitCache(const FCVOrbitRigsSettings& Orbits)
{
	Setting = Orbits;

	const FVector4 ZeroVector4 = FVector4(0.0F, 0.0F, 0.0F, 0.0F);

	float Alpha = Setting.SplineCurvature;
	CachedKnots[1].Set(0.0F, Setting.Bottom.Height, -Setting.Bottom.Radius, -1.0F);
	CachedKnots[2].Set(0.0F, Setting.Center.Height, -Setting.Center.Radius, 0.0F);
	CachedKnots[3].Set(0.0F, Setting.Top.Height, -Setting.Top.Radius, 1.0F);
	CachedKnots[0] = FMath::Lerp(CachedKnots[1] + (CachedKnots[1] - CachedKnots[2]) * 0.5F, ZeroVector4, Alpha);
	CachedKnots[4] = FMath::Lerp(CachedKnots[3] + (CachedKnots[3] - CachedKnots[2]) * 0.5F, ZeroVector4, Alpha);
	ComputeSmoothControlPoints(CachedKnots, CachedCtrl1, CachedCtrl2);
}

FVector4 FCVOrbitSplineCache::SplineValue(float Alpha) const
{
	int32 N = 1;
	if (Alpha > 0.5F)
	{
		Alpha -= 0.5F;
		N = 2;
	}
	FVector4 Location = USplineHelper::Bezier3(Alpha * 2.0F, CachedKnots[N], CachedCtrl1[N], CachedCtrl2[N], CachedKnots[N + 1]);
	Location.W = USplineHelper::Bezier1(Alpha * 2.0F, CachedKnots[N].W, CachedCtrl1[N].W, CachedCtrl2[N].W, CachedKnots[N + 1].W);
	return Location;
}

//! UCinemachineOrbitalFollow

UCinemachineOrbitalFollow::UCinemachineOrbitalFollow()
	: HorizontalAxis(0.0F, FVector2D(-180.0F, 180.0F), true, 0.0F)
	, VerticalAxis(-17.5F, FVector2D(-45.0F, 10.0F), false, -17.5F)
	, RadialAxis(1.0F, FVector2D(1.0F, 1.0F), false, 1.0F)
{
}

void UCinemachineOrbitalFollow::Validate()
{
	HorizontalAxis.Validate();
	VerticalAxis.Validate();
	RadialAxis.Validate();
}

bool UCinemachineOrbitalFollow::IsStageValid() const
{
	return IsInitialized() && IsValid(Owner) && IsValid(GetFollowTarget());
}

void UCinemachineOrbitalFollow::MutateCameraState(FCinemachineCameraState& InState, float DeltaTime)
{
	TargetTracker.InitState(this, DeltaTime, TrackerSettings.BindingMode, InState.ReferenceUp);
	if (!IsStageValid())
	{
		return;
	}

	if (DeltaTime < 0.0F)
	{
		ResetInput(FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag> { HorizontalAxis.InputAxisTag, VerticalAxis.InputAxisTag, RadialAxis.InputAxisTag }));
	}

	FVector Offset = GetCameraPoint();

	bool bInput = HorizontalAxis.TrackValueChange(GetWorld());
	bInput = VerticalAxis.TrackValueChange(GetWorld()) || bInput;
	bInput = RadialAxis.TrackValueChange(GetWorld()) || bInput;
	if (TrackerSettings.BindingMode == ECVBindingMode::LazyFollow)
	{
		HorizontalAxis.SetValueAndLastValue(0.0F);
	}

	FVector Location = FVector::ZeroVector;
	FRotator Orientation = FRotator::ZeroRotator;
	TargetTracker.TrackTarget(this, DeltaTime, InState.ReferenceUp, Offset, TrackerSettings, Location, Orientation);

	Offset = Orientation.Quaternion() * Offset;
	InState.ReferenceUp = Orientation.Quaternion() * FVector::UpVector;

	FVector TargetLocation = GetFollowTargetLocation();
	Location += TargetTracker.GetOffsetForMinimumDistance(this, Location, Offset, InState.RawOrientation.Quaternion() * FVector::ForwardVector, InState.ReferenceUp, TargetLocation);
	InState.RawLocation = Location + Offset;

	if (DeltaTime >= 0.0F && Owner->GetPreviousStateIsValid() && PreviousOffset.SizeSquared() > UE_SMALL_NUMBER && Offset.SizeSquared() > UE_SMALL_NUMBER)
	{
		InState.RotationDampingBypass = UVectorExtension::SafeFromToRotation(PreviousOffset, Offset, InState.ReferenceUp).Rotator();
	}
	PreviousOffset = Offset;

	if (HorizontalAxis.Recentering.bEnabled)
	{
		UpdateHorizontalCenter(Orientation);
	}

	HorizontalAxis.UpdateRecentering(GetWorld(), DeltaTime, bInput);
	VerticalAxis.UpdateRecentering(GetWorld(), DeltaTime, bInput);
	RadialAxis.UpdateRecentering(GetWorld(), DeltaTime, bInput);
}

bool UCinemachineOrbitalFollow::OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime)
{
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld());
	if (!IsValid(Subsystem))
	{
		return false;
	}

	ResetInput(FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag> { HorizontalAxis.InputAxisTag, VerticalAxis.InputAxisTag, RadialAxis.InputAxisTag }));
	
	TScriptInterface<ICinemachineCameraInterface> LiveCamera = GetCinemachineCameraInterface<UCinemachineVirtualCameraBaseComponent>(Owner);
	if (IsCinemachineCameraValid(FromCamera) && (GetVirtualCameraState().BlendHint & static_cast<int32>(ECVBlendHintValue::InheritPosition)) != 0 && Subsystem->IsLiveInBlend(LiveCamera))
	{
		FCinemachineCameraState State = FromCamera->GetState();
		ForceCameraLocation(State.GetFinalLocation(), State.GetFinalOrientation());
		return true;
	}
	return false;
}

void UCinemachineOrbitalFollow::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	TargetTracker.ForceCameraLocation(this, TrackerSettings.BindingMode, Location, Rotation, GetCameraPoint());
	ResetInput(FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag> { HorizontalAxis.InputAxisTag, VerticalAxis.InputAxisTag, RadialAxis.InputAxisTag }));
	if (IsValid(GetFollowTarget()))
	{
		FVector Direction = Location - GetFollowTargetLocation();
		float Distance = Direction.Size();
		if (Distance > UE_KINDA_SMALL_NUMBER)
		{
			Direction /= Distance;
			switch (OrbitStyle)
			{
			case ECVOrbitStyle::Sphere: InferAxesFromLocationSphere(Direction, Distance); break;
			case ECVOrbitStyle::ThreeRing: InferAxesFromLocationThreeRing(Direction, Distance); break;
			}
		}
	}
}

float UCinemachineOrbitalFollow::GetMaxDampTime() const
{
	return TrackerSettings.GetMaxDampTime();
}

void UCinemachineOrbitalFollow::ProcessInput(FGameplayTag InutAxisTag, float InputValue, float DeltaTime)
{
	if (HorizontalAxis.InputAxisTag.MatchesTag(InutAxisTag))
	{
		HorizontalAxis.ProcessInput(InputValue, DeltaTime);
	}
	if (VerticalAxis.InputAxisTag.MatchesTag(InutAxisTag))
	{
		VerticalAxis.ProcessInput(InputValue, DeltaTime);
	}
	if (RadialAxis.InputAxisTag.MatchesTag(InutAxisTag))
	{
		RadialAxis.ProcessInput(InputValue, DeltaTime);
	}
}

void UCinemachineOrbitalFollow::ResetInput(FGameplayTagContainer InputAxisTagContainer)
{
	if (InputAxisTagContainer.HasTagExact(HorizontalAxis.InputAxisTag))
	{
		HorizontalAxis.Reset(GetWorld());
	}
	if (InputAxisTagContainer.HasTagExact(VerticalAxis.InputAxisTag))
	{
		VerticalAxis.Reset(GetWorld());
	}
	if (InputAxisTagContainer.HasTagExact(RadialAxis.InputAxisTag))
	{
		RadialAxis.Reset(GetWorld());
	}
}

void UCinemachineOrbitalFollow::OnCancelInputRecentering(FGameplayTag InputAxisTag)
{
	if (HorizontalAxis.InputAxisTag.MatchesTagExact(InputAxisTag))
	{
		HorizontalAxis.CancelRecentering(GetWorld());
	}
	if (VerticalAxis.InputAxisTag.MatchesTagExact(InputAxisTag))
	{
		VerticalAxis.CancelRecentering(GetWorld());
	}
	if (RadialAxis.InputAxisTag.MatchesTagExact(InputAxisTag))
	{
		RadialAxis.CancelRecentering(GetWorld());
	}
}

float UCinemachineOrbitalFollow::NormalizedModifierValue()
{
	return GetCameraPoint().W;
}

FVector UCinemachineOrbitalFollow::GetLocationDamping() const
{
	return TrackerSettings.LocationDamping;
}

void UCinemachineOrbitalFollow::SetLocationDamping(const FVector& InLocationDamping)
{
	TrackerSettings.LocationDamping = InLocationDamping;
}

float UCinemachineOrbitalFollow::GetDistance() const
{
	return Radius;
}

void UCinemachineOrbitalFollow::SetDistance(float InDistance)
{
	Radius = InDistance;
}

FVector UCinemachineOrbitalFollow::GetCameraOffsetForNormalizedAxisValue(float Alpha) const
{
	return OrbitCache.SplineValue(FMath::Clamp((Alpha + 1.0F) * 0.5F, 0.0F, 1.0F));
}

FVector4 UCinemachineOrbitalFollow::GetCameraPoint()
{
	FVector Location = FVector::ZeroVector;
	float T = 0;

	switch (OrbitStyle)
	{

	case ECVOrbitStyle::Sphere:
	{
		FQuat Rotation = FQuat::MakeFromEuler(FVector(0.0F, VerticalAxis.Value, HorizontalAxis.Value));
		Location = Rotation * FVector(-Radius * RadialAxis.Value, 0.0F, 0.0F);
		T = VerticalAxis.GetNormalizedValue() * 2.0F - 1.0F;
	}
		break;

	case ECVOrbitStyle::ThreeRing:
	{
		if (OrbitCache.Setting != Orbits)
		{
			OrbitCache.UpdateOrbitCache(Orbits);
		}
		FVector4 Point = OrbitCache.SplineValue(VerticalAxis.GetNormalizedValue());
		Point.Set(Point.Z, Point.X, Point.Y, Point.W);
		Point *= RadialAxis.Value;
		Location = FQuat(FVector::UpVector, FMath::DegreesToRadians(HorizontalAxis.Value)) * Point;
		T = Point.W;
	}
		break;
	}

	if (TrackerSettings.BindingMode == ECVBindingMode::LazyFollow)
	{
		Location.X = -FMath::Abs(Location.X);
	}

	return FVector4(Location.X, Location.Y, Location.Z, T);
}

FRotator UCinemachineOrbitalFollow::GetReferenceOrientation() const
{
	return TargetTracker.PreviousReferenceOrientation.GetNormalized();
}

void UCinemachineOrbitalFollow::OnInitialize()
{
	HorizontalAxis.Restriction &= ~(static_cast<int32>(ECVRestrictionFlag::NoRecentering) | static_cast<int32>(ECVRestrictionFlag::RangeIsDriven));
	OrbitCache.UpdateOrbitCache(Orbits);

	if (UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(GetWorld()))
	{
		Subsystem->RegisterListener(HorizontalAxis.InputAxisTag, this, &UCinemachineOrbitalFollow::OnCancelInputRecentering);
		Subsystem->RegisterListener(VerticalAxis.InputAxisTag, this, &UCinemachineOrbitalFollow::OnCancelInputRecentering);
		Subsystem->RegisterListener(RadialAxis.InputAxisTag, this, &UCinemachineOrbitalFollow::OnCancelInputRecentering);
	}
}

void UCinemachineOrbitalFollow::InferAxesFromLocationSphere(FVector Direction, float Distance)
{
	FVector Up = GetVirtualCameraState().ReferenceUp;
	FRotator Orientation = TargetTracker.GetReferenceOrientation(this, TrackerSettings.BindingMode, Up);
	FVector LocalDirection = Orientation.Quaternion().Inverse() * Direction;
	FVector Euler = UVectorExtension::SafeFromToRotation(FVector::BackwardVector, LocalDirection, Up).Euler();
	VerticalAxis.Value = VerticalAxis.GetClampValue(TrackerSettings.BindingMode == ECVBindingMode::LazyFollow ? 0.0F : Euler.Y);
	HorizontalAxis.Value = HorizontalAxis.GetClampValue(Euler.Z);
	RadialAxis.Value = FMath::IsNearlyZero(Radius) ? 0.0F : RadialAxis.GetClampValue(Distance / Radius);
}

void UCinemachineOrbitalFollow::InferAxesFromLocationThreeRing(FVector Direction, float Distance)
{
	FVector Up = GetVirtualCameraState().ReferenceUp;
	FRotator Orientation = TargetTracker.GetReferenceOrientation(this, TrackerSettings.BindingMode, Up);
	FVector SplinePoint = FVector::ZeroVector;

	HorizontalAxis.Value = GetHorizontalAxis(Direction, Orientation.Quaternion(), Up);
	VerticalAxis.Value = GetVerticalAxisClosestValue(Direction, Distance, Up, SplinePoint);
	const float SplinePointDist = SplinePoint.SizeSquared();
	RadialAxis.Value = FMath::IsNearlyZero(SplinePointDist) ? 0.0F : RadialAxis.GetClampValue(Distance / SplinePointDist);
}

float UCinemachineOrbitalFollow::GetHorizontalAxis(FVector Direction, FQuat Rotation, FVector Up) const
{
	FVector Forward = FVector::VectorPlaneProject(Rotation * FVector::BackwardVector, Up);
	if (!Forward.IsNearlyZero())
	{
		return UVectorExtension::SignedAngle(Forward, FVector::VectorPlaneProject(Direction, Up), Up);
	}
	return HorizontalAxis.Value;
}

float UCinemachineOrbitalFollow::GetVerticalAxisClosestValue(FVector Direction, float Distance, FVector Up, FVector& SplinePoint)
{
	FQuat Rotation = UVectorExtension::SafeFromToRotation(Up, FVector::UpVector, Up);
	FVector LocalDirection = Rotation * Direction;
	FVector FlatDirection(LocalDirection.X, LocalDirection.Y, 0.0F);
	if (!FlatDirection.IsNearlyZero())
	{
		float Angle = UVectorExtension::SignedAngle(FlatDirection, FVector::BackwardVector, FVector::UpVector);
		LocalDirection = FQuat(FVector::UpVector, FMath::DegreesToRadians(Angle)) * LocalDirection;
	}
	LocalDirection.Y = 0.0F;
	LocalDirection.Normalize();

	float Alpha = GetSteepestDescent(LocalDirection * Distance);
	SplinePoint = OrbitCache.SplineValue(Alpha);
	return Alpha <= 0.5F
		? FMath::Lerp(VerticalAxis.Range.X, VerticalAxis.Center, MapTo01(Alpha, 0.0F, 0.5F))	// [0, 0.5] -> [0, 1] -> [Range.x, Center]
		: FMath::Lerp(VerticalAxis.Center, VerticalAxis.Range.Y, MapTo01(Alpha, 0.5F, 1.0F));	// [0.5, 1] -> [0, 1] -> [Center, Range.Y]
}

float UCinemachineOrbitalFollow::GetSteepestDescent(FVector CameraOffset)
{
	const int32 MaxIteration = 10;
	float X = GetInitialGuess(CameraOffset);
	for (int32 i = 0; i < MaxIteration; ++i)
	{
		float Angle = GetAngle(CameraOffset, X);
		float Slope = GetSlopeOfAngle(CameraOffset, X);
		if(FMath::Abs(Slope) < UE_KINDA_SMALL_NUMBER || FMath::Abs(Angle) < UE_KINDA_SMALL_NUMBER)
		{
			break;	// Best Case
		}
		X = FMath::Clamp(X - (Angle / Slope), 0.0F, 1.0F);
	}
	return X;
}

float UCinemachineOrbitalFollow::GetAngle(FVector CameraOffset, float Input) const
{
	return FMath::Abs(UVectorExtension::SignedAngle(CameraOffset, OrbitCache.SplineValue(Input), FVector::RightVector));
}

float UCinemachineOrbitalFollow::GetSlopeOfAngle(FVector CameraOffset, float Input) const
{
	const float AngleBehind = GetAngle(CameraOffset, Input - UE_KINDA_SMALL_NUMBER);
	const float AngleAhead = GetAngle(CameraOffset, Input + UE_KINDA_SMALL_NUMBER);
	return (AngleAhead - AngleBehind) / (2.0F * UE_KINDA_SMALL_NUMBER);
}

float UCinemachineOrbitalFollow::GetInitialGuess(FVector CameraLocationInRigSpace)
{
	if (OrbitCache.Setting != Orbits)
	{
		OrbitCache.UpdateOrbitCache(Orbits);
	}
	FVector4 PB = OrbitCache.SplineValue(0.0F);	// point at the bottom of spline
	FVector4 PM = OrbitCache.SplineValue(0.5F);	// point in the middle of spline
	FVector4 PT = OrbitCache.SplineValue(1.0F);	// point at the top of spline
	float Alpha1 = UVectorExtension::ClosestPointOnSegment(CameraLocationInRigSpace, PB, PM);
	float Distance1 = (FMath::Lerp(PB, PM, Alpha1) - CameraLocationInRigSpace).SizeSquared();
	float Alpha2 = UVectorExtension::ClosestPointOnSegment(CameraLocationInRigSpace, PM, PT);
	float Distance2 = (FMath::Lerp(PM, PT, Alpha2) - CameraLocationInRigSpace).SizeSquared();

	// [0,0.5]는 아래쪽에서 중간을 나타내고 [0.5,1]은 중간에서 위쪽을 나타냅니다.
	return Distance1 < Distance2 ? FMath::Lerp(0.0F, 0.5F, Alpha1) : FMath::Lerp(0.5F, 1.0F, Alpha2); // 중간에서 상단을 나타냅니다.
}

void UCinemachineOrbitalFollow::UpdateHorizontalCenter(FRotator ReferenceOrientation)
{
	FVector Forward = FVector::ForwardVector;
	switch (RecenteringTarget)
	{
	case ECVOrbitalFollowReferenceFrame::AxisCenter:
	{
		if (TrackerSettings.BindingMode == ECVBindingMode::LazyFollow)
		{
			HorizontalAxis.Center = 0.0F;
		}
		return;
	}
	case ECVOrbitalFollowReferenceFrame::TrackingTarget:
	{
		USceneComponent* FollowTarget = GetFollowTarget();
		if (IsValid(FollowTarget))
		{
			Forward = FollowTarget->GetForwardVector();
		}
	}
		break;

	case ECVOrbitalFollowReferenceFrame::LookAtTarget:
	{
		USceneComponent* LookAtTarget = GetLookAtTarget();
		if (IsValid(LookAtTarget))
		{
			Forward = LookAtTarget->GetForwardVector();
		}
	}
		break;
	}

	FVector Up = ReferenceOrientation.Quaternion() * FVector::UpVector;
	HorizontalAxis.Center = -UVectorExtension::SignedAngle(FVector::VectorPlaneProject(Forward, Up), ReferenceOrientation.Quaternion() * FVector::ForwardVector, Up);
}
