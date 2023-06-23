
#include "CinemachineCameraState.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"

static FVector Slerp(const FVector& A, const FVector& B, const float T)
{
	const float Omega = FGenericPlatformMath::Acos(FVector::DotProduct(A.GetSafeNormal(), B.GetSafeNormal()));
	const float SinOmega = FGenericPlatformMath::Sin(Omega);
	const FVector TermOne = A * (FGenericPlatformMath::Sin(Omega * (1.0f - T)) / SinOmega);
	const FVector TermTwo = B * (FGenericPlatformMath::Sin(Omega * T) / SinOmega);
	return TermOne + TermTwo;
}

static FRotator LookRotation(FVector Forward, FVector Up = FVector::UpVector)
{
	Forward.Normalize();
	FVector Right = FVector::CrossProduct(Up, Forward).GetSafeNormal();
	Up = FVector::CrossProduct(Forward, Right).GetSafeNormal();

	FMatrix RotationMatrix = FRotationMatrix::MakeFromXZ(Forward, Up);
	return RotationMatrix.Rotator();
}

static float SignedAngle(FVector V1, FVector V2, FVector Up)
{
	const FVector N1 = V1.GetSafeNormal();
	const FVector N2 = V2.GetSafeNormal();
	float Angle = FMath::RadiansToDegrees(FPlatformMath::Atan2((N1 - N2).SizeSquared(), (N1 + N2).SizeSquared())) * 2.0F;
	return FMath::Sign(Up.Dot(V1.Cross(V2))) < 0.0F ? -Angle : Angle;

}

static FQuat RotateAngleAxis(float Angle, FVector Axis)
{
    const float AngleRad = FMath::DegreesToRadians(Angle);
	Axis.Normalize();
    return FQuat(Axis * FMath::Sin(0.5f * AngleRad), FMath::Cos(0.5f * AngleRad));
}

static FVector2D GetCameraRotationToTarget(FRotator Orient, FVector LookAtDirection, FVector WorldUp)
{
	if (LookAtDirection.SizeSquared() < 1.0E-8F)
	{
		return FVector2D::ZeroVector;
	}

	FQuat ToLocal = Orient.Quaternion().Inverse();
	FVector Up = ToLocal * WorldUp;
	LookAtDirection = ToLocal * LookAtDirection;

	float AngleHorizontal = 0.0F;
	{
		FVector TargetDirectioHorizontal = FVector::VectorPlaneProject(LookAtDirection, Up);
		if (TargetDirectioHorizontal.SizeSquared() < 1.0E-8F)
		{
			FVector CurrentDirectionHoriozntal = FVector::VectorPlaneProject(FVector::ForwardVector, Up);
			if (CurrentDirectionHoriozntal.SizeSquared() < 1.0E-8F)
			{
				CurrentDirectionHoriozntal = FVector::VectorPlaneProject(CurrentDirectionHoriozntal.Dot(Up) > 0.0F ? FVector::RightVector : FVector::LeftVector, Up);
			}
			AngleHorizontal = SignedAngle(CurrentDirectionHoriozntal, TargetDirectioHorizontal, Up);
		}
	}
	
	FQuat Q = RotateAngleAxis(AngleHorizontal, Up);
	float AngleVertical = SignedAngle(Q * FVector::ForwardVector, LookAtDirection, Q * FVector::RightVector);

	return FVector2D(AngleHorizontal, AngleVertical);
}

static FRotator ApplyCameraRotation(FRotator Orient, FVector2D Rotation, FVector WorldUp)
{
	return ((RotateAngleAxis(Rotation.Y, WorldUp) * Orient.Quaternion()) * RotateAngleAxis(Rotation.X, FVector::RightVector)).Rotator();
}

static float InterpolateFieldOfView(float FieldOfViewA, float FieldOfViewB, float Alpha)
{
	float HeightA = 2.0F * FGenericPlatformMath::Tan(FMath::DegreesToRadians(FieldOfViewA) * 0.5F);
	float HeightB = 2.0F * FGenericPlatformMath::Tan(FMath::DegreesToRadians(FieldOfViewB) * 0.5F);
	float Height = FMath::Lerp(HeightA, HeightB, Alpha);
	float FieldOfView = 179.0F;
	if (Height > 0.001F)
	{
		FieldOfView = FMath::RadiansToDegrees(FGenericPlatformMath::Atan(Height * 0.5F) * 2.0F);
	}
	return FMath::Clamp(FieldOfView, FMath::Min(FieldOfViewA, FieldOfViewB), FMath::Max(FieldOfViewA, FieldOfViewB));
}

static FVector ApplyLocationBlendHint(FVector LocationA, int32 HintA, FVector LocationB, int32 HintB, FVector OriginalLocation, FVector Blended)
{
	if (((HintA | HintB) & static_cast<int32>(ECinemachineBlendHintValue::NoLocation)) == 0)
	{
		return Blended;
	}
	if (((HintA & HintB) & static_cast<int32>(ECinemachineBlendHintValue::NoLocation)) != 0)
	{
		return OriginalLocation;
	}
	if ((HintA & static_cast<int32>(ECinemachineBlendHintValue::NoLocation)) != 0)
	{
		return LocationB;
	}
	return LocationA;
}

static FRotator ApplyOrientationBlendHint(FRotator OrientationA, int32 HintA, FRotator OrientationB, int32 HintB, FRotator OriginalOrientation, FRotator Blended)
{
	if (((HintA | HintB) & static_cast<int32>(ECinemachineBlendHintValue::NoOrientation)) == 0)
	{
		return Blended;
	}
	if (((HintA & HintB) & static_cast<int32>(ECinemachineBlendHintValue::NoOrientation)) != 0)
	{
		return OriginalOrientation;
	}
	if ((HintA & static_cast<int32>(ECinemachineBlendHintValue::NoOrientation)) != 0)
	{
		return OrientationB;
	}
	return OrientationA;
}

FCinemachineCameraState FCinemachineCameraState::Lerp(const FCinemachineCameraState& A, const FCinemachineCameraState& B, float Alpha)
{
	Alpha = FMath::Clamp(Alpha, 0.0F, 1.0F);
	float AdjustedAlpha = Alpha;

	FCinemachineCameraState State;

	const int32 BlendHintA = static_cast<int32>(A.BlendHint);
	const int32 BlendHintB = static_cast<int32>(B.BlendHint);

	if (((BlendHintA & BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::NoLocation)) != 0)
	{
		State.BlendHint = static_cast<ECinemachineBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECinemachineBlendHintValue::NoLocation));
	}
	if (((BlendHintA & BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::NoOrientation)) != 0)
	{
		State.BlendHint = static_cast<ECinemachineBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECinemachineBlendHintValue::NoOrientation));
	}
	if (((BlendHintA & BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::NoLens)) != 0)
	{
		State.BlendHint = static_cast<ECinemachineBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECinemachineBlendHintValue::NoLens));
	}
	if (((BlendHintA | BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::SphericalLocationBlend)) != 0)
	{
		State.BlendHint = static_cast<ECinemachineBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECinemachineBlendHintValue::SphericalLocationBlend));
	}
	if (((BlendHintA | BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::CylindricalLocationBlend)) != 0)
	{
		State.BlendHint = static_cast<ECinemachineBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECinemachineBlendHintValue::CylindricalLocationBlend));
	}

	if (((BlendHintA | BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::NoLens)) == 0)
	{
		State.Lens = FCinemachineLensSettings::Lerp(A.Lens, B.Lens, Alpha);
	}
	else if (((BlendHintA & BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::NoLens)) == 0)
	{
		State.Lens = (BlendHintA & static_cast<int32>(ECinemachineBlendHintValue::NoLens)) != 0 ? B.Lens : A.Lens;
	}

	State.ReferenceUp = Slerp(A.ReferenceUp, B.ReferenceUp, Alpha);
	State.ShotQuality = FMath::Lerp(A.ShotQuality, B.ShotQuality, Alpha);
	State.LocationCorrection = ApplyLocationBlendHint(A.LocationCorrection, BlendHintA, B.LocationCorrection, BlendHintB, State.LocationCorrection, FMath::Lerp(A.LocationCorrection, B.LocationCorrection, Alpha));
	State.OrientationCorrection = ApplyOrientationBlendHint(A.OrientationCorrection, BlendHintA, B.OrientationCorrection, BlendHintB, State.OrientationCorrection, FMath::Lerp(A.OrientationCorrection, B.OrientationCorrection, Alpha));

	if (!A.HasLookAt() || !B.HasLookAt())
	{
		State.ReferenceLookAt = FVector::ZeroVector;
	}
	else
	{
		float FieldOfViewA = A.Lens.FieldOfView;
		float FieldOfViewB = B.Lens.FieldOfView;
		if (((BlendHintA | BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::NoLens)) == 0 && !FMath::IsNearlyEqual(FieldOfViewA, FieldOfViewB))
		{
			FCinemachineLensSettings& Lens = State.Lens;
			Lens.FieldOfView = InterpolateFieldOfView(FieldOfViewA, FieldOfViewB, Alpha);
			AdjustedAlpha = FGenericPlatformMath::Abs((Lens.FieldOfView - FieldOfViewA) / (FieldOfViewB - FieldOfViewA));
		}
		State.ReferenceLookAt = FMath::Lerp(A.ReferenceLookAt, B.ReferenceLookAt, AdjustedAlpha);
	}

	State.RawLocation = ApplyLocationBlendHint
	(
		A.RawLocation,
		BlendHintA,
		B.RawLocation,
		BlendHintB,
		State.RawLocation,
		State.InterpolateLocation
		(
			A.RawLocation,
			A.ReferenceLookAt,
			B.RawLocation,
			B.ReferenceLookAt,
			Alpha
		)
	);

	if (State.HasLookAt() && ((BlendHintA | BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::RadialAimBlend)) != 0)
	{
		State.ReferenceLookAt = State.RawLocation + Slerp(A.ReferenceLookAt - A.RawLocation, B.ReferenceLookAt - B.RawLocation, AdjustedAlpha);
	}

	FRotator NewOrient = State.RawOrientation;
	if (((BlendHintA | BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::NoOrientation)) == 0)
	{
		FVector DirectionTarget = FVector::ZeroVector;
		if (State.HasLookAt())
		{
			float Num = FMath::Min(FGenericPlatformMath::Abs(A.RawOrientation.Quaternion() | B.RawOrientation.Quaternion()), 1.0F);
			float Angle = Num > 0.999999f ? 0.0f : FGenericPlatformMath::Acos(Num) * 2.0F * 57.29578F;
			if (Angle > 0.0001F)
			{
				DirectionTarget = State.ReferenceLookAt - State.GetCorrectedLocation();
			}
		}

		if (DirectionTarget.SizeSquared() < 1.0E-8F || ((BlendHintA | BlendHintB) & static_cast<int32>(ECinemachineBlendHintValue::IgnoreLookAtTarget)) != 0)
		{
			NewOrient = FQuat::Slerp(A.RawOrientation.Quaternion(), B.RawOrientation.Quaternion(), Alpha).Rotator();
		}
		else
		{
			FVector Up = State.ReferenceUp;
			DirectionTarget.Normalize();
			if (DirectionTarget.Cross(Up).SizeSquared() < 1.0E-8F)
			{
				NewOrient = FQuat::Slerp(A.RawOrientation.Quaternion(), B.RawOrientation.Quaternion(), Alpha).Rotator();
				Up = NewOrient.Quaternion() * FVector::UpVector;
			}

			NewOrient = LookRotation(DirectionTarget, Up);
			FVector2D DeltaA = -GetCameraRotationToTarget(A.RawOrientation, A.ReferenceLookAt - A.GetCorrectedLocation(), Up);
			FVector2D DeltaB = -GetCameraRotationToTarget(B.RawOrientation, B.ReferenceLookAt - B.GetCorrectedLocation(), Up);
			NewOrient = ApplyCameraRotation(NewOrient, FMath::Lerp(DeltaA, DeltaB, AdjustedAlpha), Up);
		}
	}

	State.RawOrientation = ApplyOrientationBlendHint
	(
		A.RawOrientation,
		BlendHintA,
		B.RawOrientation,
		BlendHintB,
		State.RawOrientation,
		NewOrient
	);

	return State;
}

FVector FCinemachineCameraState::InterpolateLocation(FVector LocationA, FVector PivotA, FVector LocationB, FVector PivotB, float Alpha) const
{
	if ((static_cast<int32>(BlendHint) & static_cast<int32>(ECinemachineBlendHintValue::CylindricalLocationBlend)) != 0)
	{
		const FVector ProjectedLocationA = FVector::VectorPlaneProject(LocationA - PivotA, ReferenceUp);
		const FVector ProjectedLocationB = FVector::VectorPlaneProject(LocationB - PivotB, ReferenceUp);
		const FVector InterpolatedProjectedLocation = Slerp(ProjectedLocationA, ProjectedLocationB, Alpha);
		LocationA = (LocationA - PivotA) + InterpolatedProjectedLocation;
		LocationB = (LocationB - PivotB) + InterpolatedProjectedLocation;
	}
	else if ((static_cast<int32>(BlendHint) & static_cast<int32>(ECinemachineBlendHintValue::SphericalLocationBlend)) != 0)
	{
		const FVector InterpolatedLocation = Slerp(LocationA - PivotA, LocationB - PivotB, Alpha);
		LocationA = (LocationA - PivotA) + InterpolatedLocation;
		LocationB = (LocationB - PivotB) + InterpolatedLocation;
	}
	return FMath::Lerp(LocationA, LocationB, Alpha);
}
