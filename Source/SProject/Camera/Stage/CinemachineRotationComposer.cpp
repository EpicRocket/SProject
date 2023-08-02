

#include "CinemachineRotationComposer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/Core/CinemachineCoreSubSystem.h"
#include "Camera/Core/CinemachineVIrtualCameraBaseComponent.h"
#include "Shared/VectorExtension.h"

template<typename T>
struct TCinemachineComposerPerspectiveMatrix : public UE::Math::TMatrix<T>
{
public:
	TCinemachineComposerPerspectiveMatrix(T FOV, T Aspect, T MinZ, T MaxZ);

	// Conversion to other type.
	template<typename FArg, TEMPLATE_REQUIRES(!std::is_same_v<T, FArg>)>
	explicit TCinemachineComposerPerspectiveMatrix(const TCinemachineComposerPerspectiveMatrix<FArg>& From) : TMatrix<T>(From) {}
};

template<typename T>
TCinemachineComposerPerspectiveMatrix<T>::TCinemachineComposerPerspectiveMatrix(T FOV, T Aspect, T MinZ, T MaxZ)
	: TMatrix<T>(
		UE::Math::TPlane<T>(1.0F / FMath::Tan(FMath::DegreesToRadians(FOV) / 2.0F) / Aspect, 0.0f, 0.0f, 0.0f),
		UE::Math::TPlane<T>(0.0f, 1.0F / FMath::Tan(FMath::DegreesToRadians(FOV) / 2.0F), 0.0f, 0.0f),
		UE::Math::TPlane<T>(0.0f, 0.0f, -1 * (MaxZ + MinZ) / (MaxZ - MinZ), -1.0f),
		UE::Math::TPlane<T>(0.0f, 0.0f, -2 * MaxZ * MinZ / (MaxZ - MinZ), 0.0f)
	)
{ }

using FCinemachineComposePerspectiveMatrix = TCinemachineComposerPerspectiveMatrix<double>;

#define LOCTEXT_NAMESPACE "CinemachineRotationComposer"
template<typename T>
static FVector MultiplyPoint(UE::Math::TMatrix<T> Matrix, FVector Point)
{
	FVector Result;
	Result.X = Point.Y * Matrix.M[0][2] + Point.Z * Matrix.M[1][2] + Point.X * Matrix.M[2][2] + Matrix.M[3][2];
	Result.Z = Point.Y * Matrix.M[0][1] + Point.Z * Matrix.M[1][1] + Point.X * Matrix.M[2][1] + Matrix.M[3][1];
	Result.Y = Point.Y * Matrix.M[0][0] + Point.Z * Matrix.M[1][0] + Point.X * Matrix.M[2][0] + Matrix.M[3][0];
	float Num = Point.Y * Matrix.M[0][3] + Point.Z * Matrix.M[1][3] + Point.X * Matrix.M[2][3] + Matrix.M[3][3];
	return Result / Num;
}
#undef LOCTEXT_NAMESPACE

//! FCVFOVCache

FRect FCVFOVCache::ScreenToFOV(FRect ScreenRect, float FOV, float FOVH, float Aspect)
{
	FRect FOVRect = ScreenRect;
	FMatrix PerspectiveMatrix = FCinemachineComposePerspectiveMatrix(FOV, Aspect, UE_KINDA_SMALL_NUMBER, 2.0F).Inverse();

	FVector P = MultiplyPoint(PerspectiveMatrix, FVector(0.5f, 0.0f, (FOVRect.GetMinY() * 2.0f) - 1.0f));
	P.X = -P.X;
	float Angle = UVectorExtension::SignedAngle(FVector::ForwardVector, P, FVector::LeftVector);
	FOVRect.SetMinY(((FOV / 2.0f) + Angle) / FOV);

	P = MultiplyPoint(PerspectiveMatrix, FVector(0.5f, 0.0f, (FOVRect.GetMaxY() * 2.0f) - 1.0f));
	P.X = -P.X;
	Angle = UVectorExtension::SignedAngle(FVector::ForwardVector, P, FVector::LeftVector);
	FOVRect.SetMaxY(((FOV / 2.0f) + Angle) / FOV);

	P = MultiplyPoint(PerspectiveMatrix, FVector(0.5f, (FOVRect.GetMinX() * 2.0f) - 1.0f, 0.0f));
	P.X = -P.X;
	Angle = UVectorExtension::SignedAngle(FVector::ForwardVector, P, FVector::UpVector);
	FOVRect.SetMinX(((FOVH / 2.0f) + Angle) / FOVH);

	P = MultiplyPoint(PerspectiveMatrix, FVector(0.5f, (FOVRect.GetMaxX() * 2.0f) - 1.0f, 0.0f));
	P.X = -P.X;
	Angle = UVectorExtension::SignedAngle(FVector::ForwardVector, P, FVector::UpVector);
	FOVRect.SetMaxX(((FOVH / 2.0f) + Angle) / FOVH);

	return FOVRect;
}

void FCVFOVCache::UpdateCache(FCinemachineLensSettings Lens, FRect SoftGuide, FRect HardGuide)
{
	bool bRecalculate = !FMath::IsNearlyEqual(Aspect, Lens.AspectRatio) || SoftGuide != DeadZoneRect || HardGuide != HardLimitRect;
	float VerticalFOV = Lens.FieldOfView;
	if (!FMath::IsNearlyEqual(FOV, VerticalFOV))
	{
		bRecalculate = true;
	}
	if (bRecalculate)
	{
		FOV = VerticalFOV;
		double RadianHFOV = 2.0 * FMath::Atan(FMath::Tan(FMath::DegreesToRadians(FOV) * 0.5) * Lens.AspectRatio);
		FOVH = FMath::RadiansToDegrees(RadianHFOV);

		FOVSoftGuideRect = ScreenToFOV(SoftGuide, FOV, FOVH, Lens.AspectRatio);
		DeadZoneRect = SoftGuide;
		FOVHardGuideRect = ScreenToFOV(HardGuide, FOV, FOVH, Lens.AspectRatio);
		HardLimitRect = HardGuide;
		Aspect = Lens.AspectRatio;
	}
}

//! UCinemachineRotationComposer

bool UCinemachineRotationComposer::IsStageValid() const
{
	return IsInitialized() && IsValid(Owner) && IsValid(GetLookAtTarget());
}

void UCinemachineRotationComposer::PrePipelineMutateCameraState(FCinemachineCameraState& InState, float DeltaTime)
{
	if (IsStageValid() && InState.HasLookAt())
	{
		InState.ReferenceLookAt = GetLookAtPointAndSetTrackedPoint(InState.ReferenceLookAt, InState.ReferenceUp, DeltaTime);
	}
}

void UCinemachineRotationComposer::MutateCameraState(FCinemachineCameraState& InState, float DeltaTime)
{
	if (!IsStageValid() || !InState.HasLookAt())
	{
		return;
	}

	// 실제 타겟이 앞에 있을 때 카메라 뒤에 있는 경우 추적된 지점을 수정합니다.
	if (!(TrackedPoint - InState.ReferenceLookAt).IsNearlyZero())
	{
		FVector Mid = FMath::Lerp(InState.GetCorrectedLocation(), InState.ReferenceLookAt, 0.5F);
		FVector ToLookAt = InState.ReferenceLookAt - Mid;
		FVector ToTracked = TrackedPoint - Mid;
		if ((ToLookAt | ToTracked) < 0.0F)
		{
			float V1 = FVector::Distance(InState.ReferenceLookAt, Mid);
			float V2 = FVector::Distance(InState.ReferenceLookAt, TrackedPoint);
			float Alpha = V1 / V2;
			TrackedPoint = FMath::Lerp(InState.ReferenceLookAt, TrackedPoint, Alpha);
		}
	}

	if ((TrackedPoint - InState.GetCorrectedLocation()).Size() < UE_KINDA_SMALL_NUMBER)
	{
		if (DeltaTime >= 0.0F && Owner->GetPreviousStateIsValid())
		{
			InState.RawOrientation = CameraOrientationPrevFrame;
		}
		return;
	}

	FOVCache.UpdateCache(InState.Lens, Composition.GetDeadZoneRect(), Composition.GetHardLimitRect());

	FRotator RigOrientation = InState.RawOrientation;
	if (DeltaTime < 0.0F || !Owner->GetPreviousStateIsValid())
	{
		RigOrientation = UVectorExtension::LookRotation(RigOrientation.Quaternion() * FVector::ForwardVector, InState.ReferenceUp);
		FRect Rect = FOVCache.FOVSoftGuideRect;
		if (bCenterOnActivate)
		{
			Rect = FRect(Rect.GetCenter(), FVector2D::ZeroVector);
		}
		RotateToScreenBounds(InState, Rect, InState.ReferenceLookAt, RigOrientation, FOVCache.FOV, FOVCache.FOVH, -1.0F);
	}
	else
	{
		FVector Direction = LookAtPrevFrame - CameraLocationPrevFrame;
		if (Direction.IsNearlyZero())
		{
			RigOrientation = UVectorExtension::LookRotation(CameraOrientationPrevFrame.Quaternion() * FVector::ForwardVector, InState.ReferenceUp);
		}
		else
		{
			Direction = InState.RotationDampingBypass.Quaternion() * Direction;
			RigOrientation = UVectorExtension::LookRotation(Direction, InState.ReferenceUp);
			RigOrientation = ApplyCameraRotation(RigOrientation.Quaternion(), -ScreenOffsetPrevFrame, InState.ReferenceUp);
		}

		RotateToScreenBounds(InState, FOVCache.FOVSoftGuideRect, TrackedPoint, RigOrientation, FOVCache.FOV, FOVCache.FOVH, DeltaTime);

		if (Composition.HardLimits.bEnabled && (DeltaTime < 0.0F || Owner->LookAtTargetAttachment >(1 - UE_KINDA_SMALL_NUMBER)))
		{
			RotateToScreenBounds(InState, FOVCache.FOVHardGuideRect, InState.ReferenceLookAt, RigOrientation, FOVCache.FOV, FOVCache.FOVH, -1.0F);
		}
	}

	CameraLocationPrevFrame = InState.GetCorrectedLocation();
	LookAtPrevFrame = TrackedPoint;
	CameraOrientationPrevFrame = RigOrientation.GetNormalized();
	ScreenOffsetPrevFrame = GetCameraRotationToTarget(CameraOrientationPrevFrame.Quaternion(), LookAtPrevFrame - InState.GetCorrectedLocation(), InState.ReferenceUp);

	InState.RawOrientation = CameraOrientationPrevFrame;
}

void UCinemachineRotationComposer::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	CameraLocationPrevFrame = Location;
	CameraOrientationPrevFrame = Rotation;
}

float UCinemachineRotationComposer::GetMaxDampTime() const
{
	return FMath::Max(DampingX, DampingY);
}

FVector UCinemachineRotationComposer::GetLookAtPointAndSetTrackedPoint(FVector LookAt, FVector WorldUp, float DeltaTime)
{
	FVector Location = LookAt;
	if (IsValid(GetLookAtTarget()))
	{
		Location += GetLookAtTargetRotation().Quaternion() * TargetOffset;
	}

	if (!LookaheadSettings.bEnabled || LookaheadSettings.Time < UE_KINDA_SMALL_NUMBER)
	{
		TrackedPoint = Location;
	}
	else
	{
		bool bResetLookahead = IsStageValid() ? Owner->bLookAtTargetChanged || !Owner->GetPreviousStateIsValid() : false;
		Predictor.Smoothing = LookaheadSettings.Smoothing;
		Predictor.AddLocation(Location, bResetLookahead ? -1.0F : DeltaTime);
		FVector LocationDelta = Predictor.PredictLocationDelta(LookaheadSettings.Time);
		if (LookaheadSettings.IgnoreY)
		{
			LocationDelta = FVector::VectorPlaneProject(LocationDelta, WorldUp);
		}
		TrackedPoint = Location + LocationDelta;
	}

	return TrackedPoint;
}

void UCinemachineRotationComposer::RotateToScreenBounds(FCinemachineCameraState& InState, FRect ScreenRect, FVector TrackedLocation, FRotator& RigOrientation, float FOV, float FOVH, float DeltaTime)
{
	FVector TargetDirection = TrackedLocation - InState.GetCorrectedLocation();
	FVector2D RotationToRect = GetCameraRotationToTarget(RigOrientation.Quaternion(), TargetDirection, InState.ReferenceUp);

	// 외부에 있는 경우 ScreenRect의 가장자리로 가져옵니다. 내부에 있는 경우 그대로 둡니다.
	ClampVerticalBounds(ScreenRect, TargetDirection, InState.ReferenceUp, FOV);
	float Min = (ScreenRect.GetMinY() - 0.5F) * FOV;
	float Max = (ScreenRect.GetMaxY() - 0.5F) * FOV;
	if (RotationToRect.X < Min)
	{
		RotationToRect.X -= Min;
	}
	else if (RotationToRect.X > Max)
	{
		RotationToRect.X -= Max;
	}
	else
	{
		RotationToRect.X = 0.0F;
	}

	Min = (ScreenRect.GetMinX() - 0.5F) * FOVH;
	Max = (ScreenRect.GetMaxX() - 0.5F) * FOVH;
	if (RotationToRect.Y < Min)
	{
		RotationToRect.Y -= Min;
	}
	else if (RotationToRect.Y > Max)
	{
		RotationToRect.Y -= Max;
	}
	else
	{
		RotationToRect.Y = 0.0F;
	}

	if (IsStageValid())
	{
		if (DeltaTime >= 0.0F && Owner->GetPreviousStateIsValid())
		{
			RotationToRect.X = Owner->DetachedLookAtTargetDamp(RotationToRect.X, DampingY, DeltaTime);
			RotationToRect.Y = Owner->DetachedLookAtTargetDamp(RotationToRect.Y, DampingX, DeltaTime);
		}
	}

	RigOrientation = ApplyCameraRotation(RigOrientation.Quaternion(), RotationToRect, InState.ReferenceUp);
}

void UCinemachineRotationComposer::ClampVerticalBounds(FRect& Rect, FVector Direction, FVector Up, float FOV)
{
	float Angle = UVectorExtension::Angle(Direction, Up);
	float FOVH = (FOV / 2.0F) + 1.0F;
	const bool bFOVisZero = FOV > UE_KINDA_SMALL_NUMBER;
	if (Angle < FOVH)
	{
		float MaxY = !bFOVisZero ? 1.0F - (FOVH - Angle) / FOV : 1.0F;
		if (Rect.GetMaxY() > MaxY)
		{
			Rect.SetMinY(FMath::Min(Rect.GetMinY(), MaxY));
			Rect.SetMaxY(FMath::Min(Rect.GetMaxY(), MaxY));
		}
	}
	if (Angle > (180 - FOVH))
	{
		float MinY = !bFOVisZero ? (FOVH - (180 - Angle)) / FOV : 0.0F;
		if (MinY > Rect.GetMinY())
		{
			Rect.SetMinY(FMath::Max(Rect.GetMinY(), MinY));
			Rect.SetMaxY(FMath::Max(Rect.GetMaxY(), MinY));
		}
	}
}
