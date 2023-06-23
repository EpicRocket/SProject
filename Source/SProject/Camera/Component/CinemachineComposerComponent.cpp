

#include "CinemachineComposerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shared/LocationPredictor.h"
#include "Camera/CinemachineVirtualCameraBaseComponent.h"
#include "Camera/CinemachineCoreSubSystem.h"
#include "VectorExtension.h"

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

static FRotator LookRotation(FVector Forward, FVector Up = FVector::UpVector)
{
	Forward.Normalize();
	Up.Normalize();
	FVector Right = (Up ^ Forward).GetSafeNormal();
	Up = Forward ^ Right;
	return FQuat(FMatrix(Forward, Right, Up, FVector::ZeroVector)).Rotator();
}

static FRotator ApplyCameraRotation(const FQuat& Orient, const FVector2D& Rot, const FVector& WorldUp)
{
	FQuat Q = FQuat(FVector::RightVector, FMath::DegreesToRadians(Rot.X));
	return (FQuat(WorldUp, FMath::DegreesToRadians(Rot.Y)) * Orient * Q).Rotator();
}

static FVector2D GetCameraRotationToTarget(const FQuat& Orient, FVector LookAtDir, FVector WorldUp)
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

			FOVSoftGuideRect = ScreenToFOV(SoftGuide, FOV, FOVH, Lens.AspectRatio);
			SoftGuideRect = SoftGuide;
			FOVHardGuideRect = ScreenToFOV(HardGuide, FOV, FOVH, Lens.AspectRatio);
			HardGuideRect = HardGuide;
			Aspect = Lens.AspectRatio;
		}
	}
};

//! UCinemachineComposerComponent

UCinemachineComposerComponent::UCinemachineComposerComponent()
	: TrackedObjectOffset(FVector::ZeroVector)
	, LookaheadTime(0.0F)
	, LookaheadSmoothing(0.0F)
	, LookaheadIgnoreY(false)
	, HorizontalDamping(0.5F)
	, VerticalDamping(0.5F)
	, ScreenX(0.5F)
	, ScreenY(0.5F)
	, DeadZoneWidth(0.0F)
	, DeadZoneHeight(0.0F)
	, SoftZoneWidth(0.8F)
	, SoftZoneHeight(0.8F)
	, BiasX(0.0F)
	, BiasY(0.0F)
	, bCenterOnActivate(true)
	, TrackedPoint(FVector::ZeroVector)
	, CameraLocationPrevFrame(FVector::ZeroVector)
	, LookAtPrevFrame(FVector::ZeroVector)
	, ScreenOffsetPreFrame(FVector2D::ZeroVector)
	, CameraOrientationPrevFrame(FRotator::ZeroRotator)
{
}

void UCinemachineComposerComponent::BeginPlay()
{
	Super::BeginPlay();

	Predictor = NewObject<ULocationPredictor>(this);
	FOVCache = MakeShared<FFOVCache>();
}

void UCinemachineComposerComponent::PrePipelineMutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	if (State.HasLookAt())
	{
		State.ReferenceLookAt = GetLookAtPointAndSetTrackedPoint(State.ReferenceLookAt, State.ReferenceUp, DeltaTime);
	}
}

ECinemachineStage UCinemachineComposerComponent::GetStage() const
{
	return ECinemachineStage::Aim;
}

void UCinemachineComposerComponent::MutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	if (!IsValid(GetLookAtTarget()) || !State.HasLookAt() || !FOVCache.IsValid())
	{
		return;
	}

	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (!IsValid(VCamera))
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
			float Alpha = FVector::Distance(State.ReferenceLookAt, Mid) / FVector::Distance(State.ReferenceLookAt, TrackedPoint);
			TrackedPoint = FMath::Lerp(State.ReferenceLookAt, TrackedPoint, Alpha);
		}
	}

	if ((TrackedPoint - State.GetCorrectedLocation()).IsNearlyZero())
	{
		if (DeltaTime >= 0.0F && VCamera->GetPreviousStateIsValid())
		{
			State.RawOrientation = CameraOrientationPrevFrame;
		}
		return;
	}

	FOVCache->UpdateCache(State.Lens, GetSoftGuidRect(), GetHardGuidRect());

	FRotator RigOrientation = State.RawOrientation;
	if (DeltaTime < 0.0F || !VCamera->GetPreviousStateIsValid())
	{
		RigOrientation = UKismetMathLibrary::MakeRotFromXZ(RigOrientation.Quaternion() * FVector::ForwardVector, State.ReferenceUp);//LookRotation(RigOrientation.Quaternion() * FVector::ForwardVector, State.ReferenceUp);
		FBox2D Rect = FOVCache->FOVSoftGuideRect;
		if (bCenterOnActivate)
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
			RigOrientation = LookRotation(CameraOrientationPrevFrame.Quaternion() * FVector::ForwardVector, State.ReferenceUp);
		}
		else
		{
			Direction = FQuat::MakeFromEuler(State.LocationDampingBypass) * Direction;
			RigOrientation = LookRotation(Direction, State.ReferenceUp);
			RigOrientation = ApplyCameraRotation(RigOrientation.Quaternion(), -ScreenOffsetPreFrame, State.ReferenceUp);
		}

		RotateToScreenBounds(State, FOVCache->FOVSoftGuideRect, TrackedPoint, RigOrientation, FOVCache->FOV, FOVCache->FOVH, DeltaTime);

		if (DeltaTime < 0.0F || VCamera->LookAtTargetAttachment > (1 - UE_KINDA_SMALL_NUMBER))
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

void UCinemachineComposerComponent::OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta)
{
	if (Target == GetLookAtTarget())
	{
		CameraLocationPrevFrame += LocationDelta;
		LookAtPrevFrame += LocationDelta;
		Predictor->ApplyTransformDelta(LocationDelta);
	}
}

void UCinemachineComposerComponent::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	CameraLocationPrevFrame = Location;
	CameraOrientationPrevFrame = Rotation;
}

float UCinemachineComposerComponent::GetMaxDampTime() const
{
	return FMath::Max(HorizontalDamping, VerticalDamping);
}

FVector UCinemachineComposerComponent::GetLookAtPointAndSetTrackedPoint(FVector LookAt, FVector Up, float DeltaTime)
{
	FVector Location = LookAt;
	if (IsValid(GetLookAtTarget()))
	{
		Location += GetLookAtTargetRotation().Quaternion() * TrackedObjectOffset;
	}

	if (LookaheadTime < UE_KINDA_SMALL_NUMBER)
	{
		TrackedPoint = Location;
	}
	else
	{
		UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
		if (IsValid(VCamera))
		{
			bool bResetLookahead = VCamera->bLookAtTargetChanged || !VCamera->GetPreviousStateIsValid();
			Predictor->Smoothing = LookaheadSmoothing;
			Predictor->AddLocation(Location, bResetLookahead ? -1.0F : DeltaTime);
			FVector LocationDelta = Predictor->PredictLocationDelta(LookaheadTime);
			if (LookaheadIgnoreY)
			{
				LocationDelta = LocationDelta.ProjectOnToNormal(Up);
			}
			TrackedPoint = Location + LocationDelta;
		}
	}
	return Location;
}

FBox2D UCinemachineComposerComponent::GetSoftGuidRect()
{
	return FBox2D(FVector2D(ScreenX - DeadZoneWidth / 2.0F, ScreenY - DeadZoneHeight / 2.0F), FVector2D(ScreenX + DeadZoneWidth / 2.0F, ScreenY + DeadZoneHeight / 2.0F));
}

void UCinemachineComposerComponent::SetSoftGuidRect(FBox2D Rect)
{
	DeadZoneWidth = FMath::Clamp(Rect.Max.X, 0.0F, 2.0F);
	DeadZoneHeight = FMath::Clamp(Rect.Max.Y, 0.0F, 2.0F);
	ScreenX = FMath::Clamp(Rect.Min.X + DeadZoneWidth / 2.0F, -0.5F, 1.5F);
	ScreenY = FMath::Clamp(Rect.Min.Y + DeadZoneHeight / 2.0F, -0.5F, 1.5F);
	SoftZoneWidth = FMath::Max(SoftZoneWidth, DeadZoneWidth);
	SoftZoneHeight = FMath::Max(SoftZoneHeight, DeadZoneHeight);
}

FBox2D UCinemachineComposerComponent::GetHardGuidRect()
{
	return FBox2D(FVector2D(ScreenX - SoftZoneWidth / 2.0F, ScreenY - SoftZoneHeight / 2.0F), FVector2D(ScreenX + SoftZoneWidth / 2.0F, ScreenY + SoftZoneHeight / 2.0F)).MoveTo(FVector2D(BiasX * (SoftZoneWidth - DeadZoneWidth), BiasY * (SoftZoneHeight - DeadZoneWidth)));
}

void UCinemachineComposerComponent::SetHardGuidRect(FBox2D Rect)
{
	SoftZoneWidth = FMath::Clamp(Rect.Max.X, 0.0F, 2.0F);
	SoftZoneHeight = FMath::Clamp(Rect.Max.Y, 0.0F, 2.0F);
	DeadZoneWidth = FMath::Max(SoftZoneWidth, DeadZoneWidth);
	DeadZoneHeight = FMath::Max(SoftZoneHeight, DeadZoneHeight);
}

void UCinemachineComposerComponent::RotateToScreenBounds(FCinemachineCameraState& State, FBox2D ScreenRect, FVector TargetPoint, FRotator& RigOrientation, float FOV, float FOVH, float DeltaTime)
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

	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (IsValid(VCamera))
	{
		if (DeltaTime >= 0.0F && VCamera->GetPreviousStateIsValid())
		{
			RotationToRect.X = VCamera->DetachedLookAtTargetDamp(RotationToRect.X, VerticalDamping, DeltaTime);
			RotationToRect.Y = VCamera->DetachedLookAtTargetDamp(RotationToRect.Y, HorizontalDamping, DeltaTime);
		}
	}

	RigOrientation = ApplyCameraRotation(RigOrientation.Quaternion(), RotationToRect, State.ReferenceUp);
}

void UCinemachineComposerComponent::ClampVerticalBounds(FBox2D& Rect, FVector Direction, FVector Up, float FOV)
{
	float Angle = UVectorExtension::Angle(Direction, Up);
	float FOVH = (FOV / 2.0F) + 1.0F;
	if (Angle < FOVH)
	{
		float MaxY = 1.0F - (FOVH - Angle) / FOV;
		if (Rect.Max.Y > MaxY)
		{
			Rect.Min.Y = FMath::Min(Rect.Min.Y, MaxY);
			Rect.Max.Y = FMath::Min(Rect.Max.Y, MaxY);
		}
	}
	if (Angle > (180 - FOVH))
	{
		float MinY = (FOVH - (180 - Angle)) / FOV;
		if (Rect.Min.Y < MinY)
		{
			Rect.Min.Y = FMath::Max(Rect.Min.Y, MinY);
			Rect.Max.Y = FMath::Max(Rect.Max.Y, MinY);
		}
	}
}
