

#include "CinemachineCameraState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shared/VectorExtension.h"

#define LOCTEXT_NAMESPACE "CinemachineCameraState";
static float GetBetweenAngle(FQuat A, FQuat B)
{
	return FMath::RadiansToDegrees(FMath::Acos(FMath::Min(FMath::Abs(A | B), 1.0f)) * 2.0F);
}

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
#undef LOCTEXT_NAMESPACE

FRotator ApplyCameraRotation(const FQuat& Orientation, const FVector2D& Rot, const FVector& WorldUp)
{
	if (Rot.SizeSquared() < UE_KINDA_SMALL_NUMBER)
	{
		return Orientation.Rotator();
	}
	FQuat Q = FQuat(FVector::RightVector, FMath::DegreesToRadians(Rot.X));
	return ((FQuat(WorldUp, FMath::DegreesToRadians(Rot.Y)) * Orientation) * Q).Rotator();
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

	if (((A.BlendHint & B.BlendHint) & static_cast<int32>(ECVBlendHintValue::NoLocation)) != 0)
	{
		State.BlendHint = State.BlendHint | static_cast<int32>(ECVBlendHintValue::NoLocation);
	}
	if (((A.BlendHint & B.BlendHint) & static_cast<int32>(ECVBlendHintValue::NoOrientation)) != 0)
	{
		State.BlendHint = State.BlendHint | static_cast<int32>(ECVBlendHintValue::NoOrientation);
	}
	if (((A.BlendHint & B.BlendHint) & static_cast<int32>(ECVBlendHintValue::NoLens)) != 0)
	{
		State.BlendHint = State.BlendHint | static_cast<int32>(ECVBlendHintValue::NoLens);
	}
	if (((A.BlendHint | B.BlendHint) & static_cast<int32>(ECVBlendHintValue::SphericalLocationBlend)) != 0)
	{
		State.BlendHint = State.BlendHint | static_cast<int32>(ECVBlendHintValue::SphericalLocationBlend);
	}
	if (((A.BlendHint | B.BlendHint) & static_cast<int32>(ECVBlendHintValue::CylindricalLocationBlend)) != 0)
	{
		State.BlendHint = State.BlendHint | static_cast<int32>(ECVBlendHintValue::CylindricalLocationBlend);
	}
	if (((A.BlendHint | B.BlendHint) & static_cast<int32>(ECVBlendHintValue::FreezeWhenBlendingOut)) != 0)
	{
		State.BlendHint = State.BlendHint | static_cast<int32>(ECVBlendHintValue::FreezeWhenBlendingOut);
	}

	if (((A.BlendHint | B.BlendHint) & static_cast<int32>(ECVBlendHintValue::NoLens)) == 0)
	{
		State.Lens = FCinemachineLensSettings::Lerp(A.Lens, B.Lens, Alpha);
	}
	else if (((A.BlendHint & B.BlendHint) & static_cast<int32>(ECVBlendHintValue::NoLens)) == 0)
	{
		State.Lens = (A.BlendHint & static_cast<int32>(ECVBlendHintValue::NoLens)) != 0 ? B.Lens : A.Lens;
	}

	State.ReferenceUp = UKismetMathLibrary::Vector_SlerpNormals(A.ReferenceUp, B.ReferenceUp, Alpha);
	State.ShotQuality = FMath::Lerp(A.ShotQuality, B.ShotQuality, Alpha);
	State.LocationCorrection = ApplyLocationBlendHint(A.LocationCorrection, A.BlendHint, B.LocationCorrection, B.BlendHint, State.LocationCorrection, FMath::Lerp(A.LocationCorrection, B.LocationCorrection, Alpha));
	State.OrientationCorrection = ApplyOrientationBlendHint(A.OrientationCorrection, A.BlendHint, B.OrientationCorrection, B.BlendHint, State.OrientationCorrection, FMath::Lerp(A.OrientationCorrection, B.OrientationCorrection, Alpha));

	if (!A.HasLookAt() || !B.HasLookAt())
	{
		State.ReferenceLookAt = FVector::ZeroVector;
	}
	else
	{
		float FieldOfViewA = A.Lens.FieldOfView;
		float FieldOfViewB = B.Lens.FieldOfView;
		if (((A.BlendHint | B.BlendHint) & static_cast<int32>(ECVBlendHintValue::NoLens)) == 0 && !FMath::IsNearlyEqual(FieldOfViewA, FieldOfViewB))
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
		A.BlendHint,
		B.RawLocation,
		B.BlendHint,
		State.RawLocation,
		State.InterpolateLocation(
			A.RawLocation,
			A.ReferenceLookAt,
			B.RawLocation,
			B.ReferenceLookAt,
			Alpha));

	const FVector ALookAtToLocation = (A.ReferenceLookAt - State.RawLocation).GetSafeNormal();
	const FVector BLookAtToLocation = (B.ReferenceLookAt - State.RawLocation).GetSafeNormal();

	FVector TempLooAt = UKismetMathLibrary::Vector_SlerpNormals(ALookAtToLocation, BLookAtToLocation, AdjustedAlpha);
	FVector LookTemp = State.ReferenceLookAt;
	if (State.HasLookAt() && ((A.BlendHint | B.BlendHint) & static_cast<int32>(ECVBlendHintValue::ScreenSpaceAimWhenTargetsDiffer)) != 0)
	{
		State.ReferenceLookAt = State.RawLocation + UKismetMathLibrary::Vector_SlerpNormals(ALookAtToLocation, BLookAtToLocation, AdjustedAlpha);
	}

	FRotator NewOrient = State.RawOrientation;
	if (((A.BlendHint | B.BlendHint) & static_cast<int32>(ECVBlendHintValue::NoOrientation)) == 0)
	{
		FVector DirectionTarget = FVector::ZeroVector;
		if (State.HasLookAt()/*&& (((A.BlendHint | B.BlendHint) & static_cast<int32>(ECVBlendHintValue::ScreenSpaceAimWhenTargetsDiffer)) == 0)*/)
		{
			float Angle = GetBetweenAngle(A.RawOrientation.Quaternion(), B.RawOrientation.Quaternion());
			if (Angle > UE_KINDA_SMALL_NUMBER)
			{
				DirectionTarget = State.ReferenceLookAt - State.GetCorrectedLocation();
			}
		}

		if (DirectionTarget.IsNearlyZero() || ((A.BlendHint | B.BlendHint) & static_cast<int32>(ECVBlendHintValue::IgnoreLookAtTarget)) != 0)
		{
			NewOrient = UKismetMathLibrary::RLerp(A.RawOrientation, B.RawOrientation, Alpha, true);
		}
		else
		{
			FVector Up = State.ReferenceUp;
			DirectionTarget.Normalize();
			if ((DirectionTarget ^ Up).IsNearlyZero())
			{
				NewOrient = UKismetMathLibrary::RLerp(A.RawOrientation, B.RawOrientation, Alpha, true);;
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
		A.BlendHint,
		B.RawOrientation,
		B.BlendHint,
		State.RawOrientation,
		NewOrient);

	return State;
}

FVector FCinemachineCameraState::InterpolateLocation(FVector LocationA, FVector PivotA, FVector LocationB, FVector PivotB, float Alpha) const
{
	if ((static_cast<int32>(BlendHint) & static_cast<int32>(ECVBlendHintValue::CylindricalLocationBlend)) != 0)
	{
		const FVector ProjectedLocationA = FVector::VectorPlaneProject(LocationA - PivotA, ReferenceUp).GetSafeNormal();
		const FVector ProjectedLocationB = FVector::VectorPlaneProject(LocationB - PivotB, ReferenceUp).GetSafeNormal();
		const FVector InterpolatedProjectedLocation = UKismetMathLibrary::Vector_SlerpNormals(ProjectedLocationA, ProjectedLocationB, Alpha);
		LocationA = (LocationA - PivotA) + InterpolatedProjectedLocation;
		LocationB = (LocationB - PivotB) + InterpolatedProjectedLocation;
	}
	else if ((static_cast<int32>(BlendHint) & static_cast<int32>(ECVBlendHintValue::SphericalLocationBlend)) != 0)
	{
		const FVector ALocationToAPivot = (PivotA - LocationA).GetSafeNormal();
		const FVector BLocationToBPivot = (PivotB - LocationB).GetSafeNormal();
		const FVector InterpolatedLocation = UKismetMathLibrary::Vector_SlerpNormals(ALocationToAPivot, BLocationToBPivot, Alpha);
		LocationA = (LocationA - PivotA) + InterpolatedLocation;
		LocationB = (LocationB - PivotB) + InterpolatedLocation;
	}
	return FMath::Lerp(LocationA, LocationB, Alpha);
}
