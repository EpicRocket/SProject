

#include "CinemachineComposer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shared/LocationPredictor.h"
#include "Camera/CinemachineVirtualCameraBaseComponent.h"
#include "Camera/CinemachineCoreSubSystem.h"
#include "Shared/VectorExtension.h"

static FBox2D ScreenToFOV(FBox2D ScreenRect, float FOV, float FOVH, float AspectRatio)
{
	FBox2D FOVRect = ScreenRect;
	FMatrix PerspectiveMatrix = FPerspectiveMatrix(FMath::DegreesToRadians(FOV), AspectRatio, UE_KINDA_SMALL_NUMBER, 2.0F).Inverse();

	FVector P = PerspectiveMatrix.TransformFVector4(FVector4(0.5f, 0.0f, (FOVRect.Min.Y * 2.0f) - 1.0f, 1.0f));
	P.X = -P.X;
	float Angle = UVectorExtension::SignedAngle(FVector::ForwardVector, P, FVector::LeftVector);
	FOVRect.Min.Y = ((FOV / 2.0f) + Angle) / FOV;

	P = PerspectiveMatrix.TransformFVector4(FVector4(0.5f, 0.0f, (FOVRect.Max.Y * 2.0f) - 1.0f, 1.0f));
	P.X = -P.X;
	Angle = UVectorExtension::SignedAngle(FVector::ForwardVector, P, FVector::LeftVector);
	FOVRect.Max.Y = ((FOV / 2.0f) + Angle) / FOV;

	P = PerspectiveMatrix.TransformFVector4(FVector4(0.5f, (FOVRect.Min.X * 2.0f) - 1.0f, 0.0f, 1.0f));
	P.X = -P.X;
	Angle = UVectorExtension::SignedAngle(FVector::ForwardVector, P, FVector::UpVector);
	FOVRect.Min.X = ((FOVH / 2.0f) + Angle) / FOVH;

	P = PerspectiveMatrix.TransformFVector4(FVector4(0.5f, (FOVRect.Max.X * 2.0f) - 1.0f, 0.0f, 1.0f));
	P.X = -P.X;
	Angle = UVectorExtension::SignedAngle(FVector::ForwardVector, P, FVector::UpVector);
	FOVRect.Max.X = ((FOVH / 2.0f) + Angle) / FOVH;

	return FOVRect;
}

//! FFOVCache
struct FFOVCache
{
public:
	FBox2D FOVSoftGuideRect;
	FBox2D FOVHardGuideRect;
	float FOVH = 0.0F;
	float FOV = 0.0F;

	float Aspect = 0.0F;
	FBox2D SoftGuideRect;
	FBox2D HardGuideRect;

public:
	void UpdateCache(FCinemachineLensSettings Lens, FBox2D SoftGuide, FBox2D HardGuide)
	{
		bool bRecalculate = !FMath::IsNearlyEqual(Aspect, Lens.AspectRatio) || SoftGuideRect != SoftGuide || HardGuideRect != HardGuide;
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

			if (FOV > UE_KINDA_SMALL_NUMBER && FOVH > UE_KINDA_SMALL_NUMBER)
			{
				FOVSoftGuideRect = ScreenToFOV(SoftGuide, FOV, FOVH, Lens.AspectRatio);
				FOVHardGuideRect = ScreenToFOV(HardGuide, FOV, FOVH, Lens.AspectRatio);
			}
			else
			{
				FOVSoftGuideRect = FBox2D(ForceInitToZero);
				FOVHardGuideRect = FBox2D(ForceInitToZero);
			}
			SoftGuideRect = SoftGuide;
			HardGuideRect = HardGuide;
			Aspect = Lens.AspectRatio;
		}
	}
};

//! UCinemachineComposer

UCinemachineComposer::UCinemachineComposer()
	: TrackedPoint(FVector::ZeroVector)
{
}

void UCinemachineComposer::PrePipelineMutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	if (State.HasLookAt())
	{
		State.ReferenceLookAt = GetLookAtPointAndSetTrackedPoint(State.ReferenceLookAt, State.ReferenceUp, DeltaTime);
	}
}

ECVStage UCinemachineComposer::GetStage() const
{
	return ECVStage::Aim;
}

void UCinemachineComposer::MutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	if (!IsValid(GetLookAtTarget()) || !State.HasLookAt() || !FOVCache.IsValid())
	{
		return;
	}

	if (!IsValid(Owner))
	{
		return;
	}

	if (!(TrackedPoint - State.ReferenceLookAt).IsNearlyZero())
	{
		FVector Mid = FMath::Lerp(State.GetCorrectedLocation(), State.ReferenceLookAt, 0.5F);
		FVector ToLookAt = State.ReferenceLookAt - Mid;
		FVector ToTracked = TrackedPoint - Mid;
		if ((ToLookAt | ToTracked) < 0.0F)
		{
			float V1 = FVector::Distance(State.ReferenceLookAt, Mid);
			float V2 = FVector::Distance(State.ReferenceLookAt, TrackedPoint);
			float Alpha = V1 / V2;
			TrackedPoint = FMath::Lerp(State.ReferenceLookAt, TrackedPoint, Alpha);
		}
	}

	if ((TrackedPoint - State.GetCorrectedLocation()).IsNearlyZero())
	{
		if (DeltaTime >= 0.0F && Owner->GetPreviousStateIsValid())
		{
			State.RawOrientation = CameraOrientationPrevFrame;
		}
		return;
	}

	FOVCache->UpdateCache(State.Lens, GetSoftGuidRect(), GetHardGuidRect());

	FRotator RigOrientation = State.RawOrientation;
	if (DeltaTime < 0.0F || !Owner->GetPreviousStateIsValid())
	{
		RigOrientation = UVectorExtension::LookRotation(RigOrientation.Quaternion() * FVector::ForwardVector, State.ReferenceUp);
		FBox2D Rect = FOVCache->FOVSoftGuideRect;
		if (ComposerData.bCenterOnActivate)
		{
			Rect = FBox2D(Rect.GetCenter(), Rect.GetCenter());
		}
		RotateToScreenBounds(State, Rect, State.ReferenceLookAt, RigOrientation, FOVCache->FOV, FOVCache->FOVH, -1.0F);
	}
	else
	{
		FVector Direction = LookAtPrevFrame - CameraLocationPrevFrame;
		if (Direction.IsNearlyZero())
		{
			RigOrientation = UVectorExtension::LookRotation(CameraOrientationPrevFrame.Quaternion() * FVector::ForwardVector, State.ReferenceUp);
		}
		else
		{
			Direction = FQuat::MakeFromEuler(State.LocationDampingBypass) * Direction;
			RigOrientation = UVectorExtension::LookRotation(Direction, State.ReferenceUp);
			RigOrientation = ApplyCameraRotation(RigOrientation.Quaternion(), -ScreenOffsetPreFrame, State.ReferenceUp);
		}

		RotateToScreenBounds(State, FOVCache->FOVSoftGuideRect, TrackedPoint, RigOrientation, FOVCache->FOV, FOVCache->FOVH, DeltaTime);

		if (DeltaTime < 0.0F || Owner->LookAtTargetAttachment > (1 - UE_KINDA_SMALL_NUMBER))
		{
			RotateToScreenBounds(State, FOVCache->FOVHardGuideRect, State.ReferenceLookAt, RigOrientation, FOVCache->FOV, FOVCache->FOVH, -1.0F);
		}
	}

	CameraLocationPrevFrame = State.GetCorrectedLocation();
	LookAtPrevFrame = TrackedPoint;
	CameraOrientationPrevFrame = RigOrientation.GetNormalized();
	ScreenOffsetPreFrame = GetCameraRotationToTarget(CameraOrientationPrevFrame.Quaternion(), LookAtPrevFrame - State.GetCorrectedLocation(), State.ReferenceUp);

	State.RawOrientation = CameraOrientationPrevFrame;
}

void UCinemachineComposer::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	CameraLocationPrevFrame = Location;
	CameraOrientationPrevFrame = Rotation;
}

float UCinemachineComposer::GetMaxDampTime() const
{
	return FMath::Max(ComposerData.HorizontalDamping, ComposerData.VerticalDamping);
}

FVector UCinemachineComposer::GetLookAtPointAndSetTrackedPoint(FVector LookAt, FVector Up, float DeltaTime)
{
	FVector Location = LookAt;
	if (IsValid(GetLookAtTarget()))
	{
		Location += GetLookAtTargetRotation().Quaternion() * ComposerData.TrackedObjectOffset;
	}

	if (ComposerData.LookaheadTime < UE_KINDA_SMALL_NUMBER)
	{
		TrackedPoint = Location;
	}
	else
	{
		if (IsValid(Owner))
		{
			bool bResetLookahead = Owner->bLookAtTargetChanged || !Owner->GetPreviousStateIsValid();
			Predictor->Smoothing = ComposerData.LookaheadSmoothing;
			Predictor->AddLocation(Location, bResetLookahead ? -1.0F : DeltaTime);
			FVector LocationDelta = Predictor->PredictLocationDelta(ComposerData.LookaheadTime);
			if (ComposerData.LookaheadIgnoreY)
			{
				LocationDelta = FVector::VectorPlaneProject(LocationDelta, Up);
			}
			TrackedPoint = Location + LocationDelta;
		}
	}
	return Location;
}

FBox2D UCinemachineComposer::GetSoftGuidRect()
{
	return FBox2D(FVector2D(ComposerData.ScreenX - ComposerData.DeadZoneWidth / 2.0F, ComposerData.ScreenY - ComposerData.DeadZoneHeight / 2.0F), FVector2D(ComposerData.ScreenX + ComposerData.DeadZoneWidth / 2.0F, ComposerData.ScreenY + ComposerData.DeadZoneHeight / 2.0F));
}

void UCinemachineComposer::SetSoftGuidRect(FBox2D Rect)
{
	ComposerData.DeadZoneWidth = FMath::Clamp(Rect.Max.X, 0.0F, 2.0F);
	ComposerData.DeadZoneHeight = FMath::Clamp(Rect.Max.Y, 0.0F, 2.0F);
	ComposerData.ScreenX = FMath::Clamp(Rect.Min.X + ComposerData.DeadZoneWidth / 2.0F, -0.5F, 1.5F);
	ComposerData.ScreenY = FMath::Clamp(Rect.Min.Y + ComposerData.DeadZoneHeight / 2.0F, -0.5F, 1.5F);
	ComposerData.SoftZoneWidth = FMath::Max(ComposerData.SoftZoneWidth, ComposerData.DeadZoneWidth);
	ComposerData.SoftZoneHeight = FMath::Max(ComposerData.SoftZoneHeight, ComposerData.DeadZoneHeight);
}

FBox2D UCinemachineComposer::GetHardGuidRect()
{
	return FBox2D(
			   FVector2D(ComposerData.ScreenX - ComposerData.SoftZoneWidth / 2.0F, ComposerData.ScreenY - ComposerData.SoftZoneHeight / 2.0F),
			   FVector2D(ComposerData.ScreenX + ComposerData.SoftZoneWidth / 2.0F, ComposerData.ScreenY + ComposerData.SoftZoneHeight / 2.0F))
		.MoveTo(FVector2D(ComposerData.BiasX * (ComposerData.SoftZoneWidth - ComposerData.DeadZoneWidth), ComposerData.BiasY * (ComposerData.SoftZoneHeight - ComposerData.DeadZoneWidth)));
}

void UCinemachineComposer::SetHardGuidRect(FBox2D Rect)
{
	ComposerData.SoftZoneWidth = FMath::Clamp(Rect.Max.X, 0.0F, 2.0F);
	ComposerData.SoftZoneHeight = FMath::Clamp(Rect.Max.Y, 0.0F, 2.0F);
	ComposerData.DeadZoneWidth = FMath::Max(ComposerData.SoftZoneWidth, ComposerData.DeadZoneWidth);
	ComposerData.DeadZoneHeight = FMath::Max(ComposerData.SoftZoneHeight, ComposerData.DeadZoneHeight);
}

void UCinemachineComposer::RotateToScreenBounds(FCinemachineCameraState& State, FBox2D ScreenRect, FVector TargetPoint, FRotator& RigOrientation, float FOV, float FOVH, float DeltaTime)
{
	FVector TargetDirection = TargetPoint - State.GetCorrectedLocation();
	FVector2D RotationToRect = GetCameraRotationToTarget(RigOrientation.Quaternion(), TargetDirection, State.ReferenceUp);

	ClampVerticalBounds(ScreenRect, TargetDirection, State.ReferenceUp, FOV);

	float Min = (ScreenRect.Min.Y - 0.5F) * FOV;
	float Max = (ScreenRect.Max.Y - 0.5F) * FOV;
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

	Min = (ScreenRect.Min.X - 0.5F) * FOVH;
	Max = (ScreenRect.Max.X - 0.5F) * FOVH;
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

	if (IsValid(Owner))
	{
		if (DeltaTime >= 0.0F && Owner->GetPreviousStateIsValid())
		{
			RotationToRect.X = Owner->DetachedLookAtTargetDamp(RotationToRect.X, ComposerData.VerticalDamping, DeltaTime);
			RotationToRect.Y = Owner->DetachedLookAtTargetDamp(RotationToRect.Y, ComposerData.HorizontalDamping, DeltaTime);
		}
	}

	RigOrientation = ApplyCameraRotation(RigOrientation.Quaternion(), RotationToRect, State.ReferenceUp);
}

void UCinemachineComposer::ClampVerticalBounds(FBox2D& Rect, FVector Direction, FVector Up, float FOV)
{
	float Angle = UVectorExtension::Angle(Direction, Up);
	float FOVH = (FOV / 2.0F) + 1.0F;
	if (Angle < FOVH)
	{
		float MaxY = FOV > UE_KINDA_SMALL_NUMBER ? 1.0F - (FOVH - Angle) / FOV : 1.0F;
		if (Rect.Max.Y > MaxY)
		{
			Rect.Min.Y = FMath::Min(Rect.Min.Y, MaxY);
			Rect.Max.Y = FMath::Min(Rect.Max.Y, MaxY);
		}
	}
	if (Angle > (180 - FOVH))
	{
		float MinY = FOV > UE_KINDA_SMALL_NUMBER ? (FOVH - (180 - Angle)) / FOV : 1.0F;
		if (Rect.Min.Y < MinY)
		{
			Rect.Min.Y = FMath::Max(Rect.Min.Y, MinY);
			Rect.Max.Y = FMath::Max(Rect.Max.Y, MinY);
		}
	}
}

void UCinemachineComposer::OnInitialize()
{
	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (HasAllFlags(RF_Transient))
	{
		Flags |= RF_Transient;
	}
	Predictor = NewObject<ULocationPredictor>(this, TEXT("Predictor(CV)"), Flags);
	FOVCache = MakeShared<FFOVCache>();
}
