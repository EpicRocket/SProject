

#include "CinemachineCameraState.h"
#include "Shared/VectorExtension.h"

static float InterpolateFieldOfView(float FieldOfViewA, float FieldOfViewB, float Alpha)
{
	float HeightA = 2.0F * FMath::Tan(FMath::DegreesToRadians(FieldOfViewA) * 0.5F);
	float HeightB = 2.0F * FMath::Tan(FMath::DegreesToRadians(FieldOfViewB) * 0.5F);
	float Height = FMath::Lerp(HeightA, HeightB, Alpha);
	float FieldOfView = 179.0F;
	if (Height > 0.001F)
	{
		FieldOfView = FMath::RadiansToDegrees(FMath::Atan(Height * 0.5F) * 2.0F);
	}
	return FMath::Clamp(FieldOfView, FMath::Min(FieldOfViewA, FieldOfViewB), FMath::Max(FieldOfViewA, FieldOfViewB));
}

static FVector ApplyLocationBlendHint(FVector LocationA, int32 HintA, FVector LocationB, int32 HintB, FVector OriginalLocation, FVector Blended)
{
	if (((HintA | HintB) & static_cast<int32>(ECVBlendHintValue::NoLocation)) == 0)
	{
		return Blended;
	}
	if (((HintA & HintB) & static_cast<int32>(ECVBlendHintValue::NoLocation)) != 0)
	{
		return OriginalLocation;
	}
	if ((HintA & static_cast<int32>(ECVBlendHintValue::NoLocation)) != 0)
	{
		return LocationB;
	}
	return LocationA;
}

static FRotator ApplyOrientationBlendHint(FRotator OrientationA, int32 HintA, FRotator OrientationB, int32 HintB, FRotator OriginalOrientation, FRotator Blended)
{
	if (((HintA | HintB) & static_cast<int32>(ECVBlendHintValue::NoOrientation)) == 0)
	{
		return Blended;
	}
	if (((HintA & HintB) & static_cast<int32>(ECVBlendHintValue::NoOrientation)) != 0)
	{
		return OriginalOrientation;
	}
	if ((HintA & static_cast<int32>(ECVBlendHintValue::NoOrientation)) != 0)
	{
		return OrientationB;
	}
	return OrientationA;
}

FRotator ApplyCameraRotation(const FQuat& Orient, const FVector2D& Rot, const FVector& WorldUp)
{
	FQuat Q = FQuat(FVector::RightVector, FMath::DegreesToRadians(Rot.X));
	return (FQuat(WorldUp, FMath::DegreesToRadians(Rot.Y)) * Orient * Q).Rotator();
}

FVector2D GetCameraRotationToTarget(const FQuat& Orient, FVector LookAtDir, FVector WorldUp)
{
	if (LookAtDir.IsNearlyZero())
	{
		return FVector2D::ZeroVector;
	}

	FQuat ToLocal = Orient.Inverse();
	FVector Up = ToLocal * WorldUp;
	LookAtDir = ToLocal * LookAtDir;

	float AngleH = 0;
	{
		FVector TargetDirH = FVector::VectorPlaneProject(LookAtDir, Up);
		if (!TargetDirH.IsNearlyZero())
		{
			FVector CurrentDirH = FVector::VectorPlaneProject(FVector::ForwardVector, Up);
			if (CurrentDirH.IsNearlyZero())
			{
				if (FVector::DotProduct(CurrentDirH, Up) > 0)
				{
					CurrentDirH = FVector::VectorPlaneProject(FVector::UpVector, Up);
				}
				else
				{
					CurrentDirH = FVector::VectorPlaneProject(FVector::DownVector, Up);
				}
			}
			AngleH = UVectorExtension::SignedAngle(CurrentDirH, TargetDirH, Up);
		}
	}
	FQuat Q = FQuat(Up, FMath::DegreesToRadians(AngleH));

	float AngleV = UVectorExtension::SignedAngle(Q * FVector::ForwardVector, LookAtDir, Q * FVector::RightVector);

	return FVector2D(AngleV, AngleH);
}

FCinemachineCameraState FCinemachineCameraState::Lerp(const FCinemachineCameraState& A, const FCinemachineCameraState& B, float Alpha)
{
	Alpha = FMath::Clamp(Alpha, 0.0F, 1.0F);
	float AdjustedAlpha = Alpha;

	FCinemachineCameraState State;

	const int32 BlendHintA = static_cast<int32>(A.BlendHint);
	const int32 BlendHintB = static_cast<int32>(B.BlendHint);

	if (((BlendHintA & BlendHintB) & static_cast<int32>(ECVBlendHintValue::NoLocation)) != 0)
	{
		State.BlendHint = static_cast<ECVBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECVBlendHintValue::NoLocation));
	}
	if (((BlendHintA & BlendHintB) & static_cast<int32>(ECVBlendHintValue::NoOrientation)) != 0)
	{
		State.BlendHint = static_cast<ECVBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECVBlendHintValue::NoOrientation));
	}
	if (((BlendHintA & BlendHintB) & static_cast<int32>(ECVBlendHintValue::NoLens)) != 0)
	{
		State.BlendHint = static_cast<ECVBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECVBlendHintValue::NoLens));
	}
	if (((BlendHintA | BlendHintB) & static_cast<int32>(ECVBlendHintValue::SphericalLocationBlend)) != 0)
	{
		State.BlendHint = static_cast<ECVBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECVBlendHintValue::SphericalLocationBlend));
	}
	if (((BlendHintA | BlendHintB) & static_cast<int32>(ECVBlendHintValue::CylindricalLocationBlend)) != 0)
	{
		State.BlendHint = static_cast<ECVBlendHintValue>(static_cast<int32>(State.BlendHint) | static_cast<int32>(ECVBlendHintValue::CylindricalLocationBlend));
	}

	if (((BlendHintA | BlendHintB) & static_cast<int32>(ECVBlendHintValue::NoLens)) == 0)
	{
		State.Lens = FCinemachineLensSettings::Lerp(A.Lens, B.Lens, Alpha);
	}
	else if (((BlendHintA & BlendHintB) & static_cast<int32>(ECVBlendHintValue::NoLens)) == 0)
	{
		State.Lens = (BlendHintA & static_cast<int32>(ECVBlendHintValue::NoLens)) != 0 ? B.Lens : A.Lens;
	}

	State.ReferenceUp = UVectorExtension::Slerp(A.ReferenceUp, B.ReferenceUp, Alpha);
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
		if (((BlendHintA | BlendHintB) & static_cast<int32>(ECVBlendHintValue::NoLens)) == 0 && !FMath::IsNearlyEqual(FieldOfViewA, FieldOfViewB))
		{
			FCinemachineLensSettings& Lens = State.Lens;
			Lens.FieldOfView = InterpolateFieldOfView(FieldOfViewA, FieldOfViewB, Alpha);
			const float MixFOV = (FieldOfViewB - FieldOfViewA); // B에서 A로의 FOV 변화량
			AdjustedAlpha = FMath::IsNearlyZero(MixFOV) ? 0.0F : FMath::Abs((Lens.FieldOfView - FieldOfViewA) / MixFOV);
		}
		State.ReferenceLookAt = FMath::Lerp(A.ReferenceLookAt, B.ReferenceLookAt, AdjustedAlpha);
	}

	State.RawLocation = ApplyLocationBlendHint(
		A.RawLocation,
		BlendHintA,
		B.RawLocation,
		BlendHintB,
		State.RawLocation,
		State.InterpolateLocation(
			A.RawLocation,
			A.ReferenceLookAt,
			B.RawLocation,
			B.ReferenceLookAt,
			Alpha));

	FVector TempLooAt = UVectorExtension::Slerp(A.ReferenceLookAt - State.RawLocation, B.ReferenceLookAt - State.RawLocation, AdjustedAlpha);
	FVector LookTemp = State.ReferenceLookAt;
	if (State.HasLookAt() && ((BlendHintA | BlendHintB) & static_cast<int32>(ECVBlendHintValue::RadialAimBlend)) != 0)
	{
		State.ReferenceLookAt = State.RawLocation + UVectorExtension::Slerp(A.ReferenceLookAt - State.RawLocation, B.ReferenceLookAt - State.RawLocation, AdjustedAlpha);
	}

	FRotator NewOrient = State.RawOrientation;
	if (((BlendHintA | BlendHintB) & static_cast<int32>(ECVBlendHintValue::NoOrientation)) == 0)
	{
		FVector DirectionTarget = FVector::ZeroVector;
		if (State.HasLookAt() && (((BlendHintA | BlendHintB) & static_cast<int32>(ECVBlendHintValue::RadialAimBlend)) != 0))
		{
			float Angle = FMath::Min(A.RawOrientation.Quaternion() | B.RawOrientation.Quaternion(), 1.0F);
			Angle = Angle > 0.999999F ? 0.0F : FMath::RadiansToDegrees(FMath::Acos(Angle) * 2.0F);
			if (Angle > UE_KINDA_SMALL_NUMBER)
			{
				DirectionTarget = State.ReferenceLookAt - State.GetCorrectedLocation();
			}
		}

		if (DirectionTarget.IsNearlyZero() || ((BlendHintA | BlendHintB) & static_cast<int32>(ECVBlendHintValue::IgnoreLookAtTarget)) != 0)
		{
			NewOrient = FMath::RInterpTo(A.RawOrientation, B.RawOrientation, Alpha, 1.0F);
		}
		else
		{
			FVector Up = State.ReferenceUp;
			DirectionTarget.Normalize();
			if ((DirectionTarget ^ Up).IsNearlyZero())
			{
				NewOrient = FMath::RInterpTo(A.RawOrientation, B.RawOrientation, Alpha, 1.0F);
				Up = NewOrient.Quaternion() * FVector::UpVector;
			}

			NewOrient = UVectorExtension::LookRotation(DirectionTarget, Up);
			FVector2D DeltaA = -GetCameraRotationToTarget(A.RawOrientation.Quaternion(), A.ReferenceLookAt - A.GetCorrectedLocation(), Up);
			FVector2D DeltaB = -GetCameraRotationToTarget(B.RawOrientation.Quaternion(), B.ReferenceLookAt - B.GetCorrectedLocation(), Up);
			NewOrient = ApplyCameraRotation(NewOrient.Quaternion(), FMath::Lerp(DeltaA, DeltaB, AdjustedAlpha), Up);
		}
	}

	State.RawOrientation = ApplyOrientationBlendHint(
		A.RawOrientation,
		BlendHintA,
		B.RawOrientation,
		BlendHintB,
		State.RawOrientation,
		NewOrient);

	return State;
}

FVector FCinemachineCameraState::InterpolateLocation(FVector LocationA, FVector PivotA, FVector LocationB, FVector PivotB, float Alpha) const
{
	if ((static_cast<int32>(BlendHint) & static_cast<int32>(ECVBlendHintValue::CylindricalLocationBlend)) != 0)
	{
		const FVector ProjectedLocationA = FVector::VectorPlaneProject(LocationA - PivotA, ReferenceUp);
		const FVector ProjectedLocationB = FVector::VectorPlaneProject(LocationB - PivotB, ReferenceUp);
		const FVector InterpolatedProjectedLocation = UVectorExtension::Slerp(ProjectedLocationA, ProjectedLocationB, Alpha);
		LocationA = (LocationA - PivotA) + InterpolatedProjectedLocation;
		LocationB = (LocationB - PivotB) + InterpolatedProjectedLocation;
	}
	else if ((static_cast<int32>(BlendHint) & static_cast<int32>(ECVBlendHintValue::SphericalLocationBlend)) != 0)
	{
		const FVector InterpolatedLocation = UVectorExtension::Slerp(LocationA - PivotA, LocationB - PivotB, Alpha);
		LocationA = (LocationA - PivotA) + InterpolatedLocation;
		LocationB = (LocationB - PivotB) + InterpolatedLocation;
	}
	return FMath::Lerp(LocationA, LocationB, Alpha);
}
