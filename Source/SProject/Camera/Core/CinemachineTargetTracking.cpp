

#include "CinemachineTargetTracking.h"
#include "Kismet/KismetMathLibrary.h"
#include "CinemachineBaseStage.h"
#include "CinemachineVirtualCameraBaseComponent.h"
#include "Shared/VectorExtension.h"

//! FCinemachineTracker

void FCinemachineTracker::InitState(UCinemachineBaseStage* Stage, float DeltaTime, ECVBindingMode BindingMode, FVector WorldUp)
{
	if (!IsValid(Stage) || !Stage->IsStageValid())
	{
		return;
	}
	bool bPreviousStateValid = DeltaTime >= 0.0F && Stage->Owner->GetPreviousStateIsValid();
	if (PreviousTarget != Stage->GetFollowTarget() || !bPreviousStateValid)
	{
		PreviousTarget = Stage->GetFollowTarget();
		TargetOrientationOnAssign = Stage->GetFollowTargetRotation();
	}
	if (!bPreviousStateValid)
	{
		PreviousTargetLocation = Stage->GetFollowTargetLocation();
		PreviousReferenceOrientation = GetReferenceOrientation(Stage, BindingMode, WorldUp);
	}
}

FRotator FCinemachineTracker::GetReferenceOrientation(UCinemachineBaseStage* Stage, ECVBindingMode BindingMode, FVector WorldUp) const
{
	if (!IsValid(Stage) || !Stage->IsStageValid() || BindingMode == ECVBindingMode::WorldSpace)
	{
		return FRotator::ZeroRotator;
	}

	USceneComponent* FollowTarget = Stage->GetFollowTarget();
	if (IsValid(FollowTarget))
	{
		FRotator TargetOrientation = FollowTarget->GetComponentRotation();
		switch(BindingMode)
		{
		case ECVBindingMode::LockToTargetOnAssign: return TargetOrientationOnAssign;
		
		case ECVBindingMode::LockToTargetWithWorldUp:
		{
			FVector Forward = FVector::VectorPlaneProject(TargetOrientation.Quaternion() * FVector::ForwardVector, WorldUp);
			if (Forward.IsNearlyZero())
			{
				break;
			}
			return UVectorExtension::LookRotation(Forward, WorldUp);
		}
			break;

		case ECVBindingMode::LockToTargetNoRoll: return UVectorExtension::LookRotation(TargetOrientation.Quaternion() * FVector::ForwardVector, WorldUp);

		case ECVBindingMode::LockToTarget: return TargetOrientation;

		case ECVBindingMode::LazyFollow:
		{
			FVector Forward = FVector::VectorPlaneProject(Stage->GetFollowTargetLocation() - Stage->GetVirtualCameraState().RawLocation, WorldUp);
			if (Forward.IsNearlyZero())
			{
				break;
			}
			return UVectorExtension::LookRotation(Forward, WorldUp);
		}
			break;
		}
	}
	return PreviousReferenceOrientation.GetNormalized();
}

void FCinemachineTracker::TrackTarget(UCinemachineBaseStage* Stage, float DeltaTime, FVector WorldUp, FVector DesiredCameraOffset, FCinemachineTrackerSettings& InSettings, FVector& OutTargetLocation, FRotator& OutTargetOrientation)
{
	if (!IsValid(Stage) || !Stage->IsStageValid())
	{
		return;
	}

	FRotator TargetOrientation = GetReferenceOrientation(Stage, InSettings.BindingMode, WorldUp);
	FRotator DampedOrientation = TargetOrientation;
	bool bPrevStateValid = DeltaTime >= 0.0F && Stage->Owner->GetPreviousStateIsValid();
	if (bPrevStateValid)
	{
		if (InSettings.AngularDampingMode == ECVAngularDampingMode::Quaternion && InSettings.BindingMode == ECVBindingMode::LockToTarget)
		{
			float Alpha = Stage->Owner->DetachedFollowTargetDamp(1.0F, InSettings.QuaternionDamping, DeltaTime);
			DampedOrientation = UKismetMathLibrary::RLerp(PreviousReferenceOrientation, TargetOrientation, Alpha, true);
		}
		else if (InSettings.BindingMode != ECVBindingMode::LazyFollow)
		{
			FVector Relative = (PreviousReferenceOrientation.GetInverse().Quaternion() * TargetOrientation.Quaternion()).Euler();
			for (int32 i = 0; i < 3; ++i)
			{
				if (Relative[i] > 180.0F)
				{
					Relative[i] -= 360.0F;
				}
				if (FMath::Abs(Relative[i]) < 0.01F) // correct for precision drift
				{
					Relative[i] = 0.0F;
				}
			}
			Relative = Stage->Owner->DetachedFollowTargetDamp(Relative, InSettings.GetEffectiveRotationDamping(), DeltaTime);
			DampedOrientation = (PreviousReferenceOrientation.Quaternion() * FQuat::MakeFromEuler(Relative)).Rotator();
		}
	}

	PreviousReferenceOrientation = DampedOrientation;

	FVector TargetLocation = Stage->GetFollowTargetLocation();
	FVector CurrentLocation = PreviousTargetLocation;
	FVector SelectedPreviousOffset = bPrevStateValid ? PreviousOffset : DesiredCameraOffset;
	FVector OffsetDelta = DesiredCameraOffset - SelectedPreviousOffset;
	if (OffsetDelta.SizeSquared() > 0.01F)
	{
		FQuat Q = UVectorExtension::SafeFromToRotation(PreviousOffset, DesiredCameraOffset, WorldUp);
		CurrentLocation = TargetLocation + Q * (PreviousTargetLocation - TargetLocation);
	}
	PreviousOffset = DesiredCameraOffset;

	FVector LocationDelta = TargetLocation - CurrentLocation;
	if (bPrevStateValid)
	{
		FRotator DampingSpace = DesiredCameraOffset.IsNearlyZero() ? Stage->GetVirtualCameraState().RawOrientation : UVectorExtension::LookRotation(DampedOrientation.Quaternion() * DesiredCameraOffset, WorldUp);
		FVector LocalDelta = DampingSpace.GetInverse().Quaternion() * LocationDelta;
		LocalDelta = Stage->Owner->DetachedFollowTargetDamp(LocalDelta, InSettings.GetEffectiveLocationDamping(), DeltaTime);
		LocationDelta = DampingSpace.Quaternion() * LocalDelta;
	}
	CurrentLocation += LocationDelta;

	OutTargetLocation = PreviousTargetLocation = CurrentLocation;
	OutTargetOrientation = DampedOrientation;
}

FVector FCinemachineTracker::GetOffsetForMinimumDistance(UCinemachineBaseStage* Stage, FVector DampedTargetLocation, FVector CameraOffset, FVector CameraForward, FVector WorldUp, FVector ActualTargetLocation)
{
	if (!IsValid(Stage) || !Stage->IsStageValid())
	{
		return FVector::ZeroVector;
	}

	FVector LocationOffset = FVector::ZeroVector;
	if (Stage->Owner->FollowTargetAttachment > 1 - UE_KINDA_SMALL_NUMBER)
	{
		CameraOffset = FVector::VectorPlaneProject(CameraOffset, WorldUp);
		float MinDistance = CameraOffset.Size() * 0.2F;
		if (MinDistance > 0.0F)
		{
			ActualTargetLocation = FVector::VectorPlaneProject(ActualTargetLocation, WorldUp);
			DampedTargetLocation = FVector::VectorPlaneProject(DampedTargetLocation, WorldUp);
			FVector CamerLocation = DampedTargetLocation + CameraOffset;
			float Distance = (ActualTargetLocation - CamerLocation) | (DampedTargetLocation - CamerLocation).GetSafeNormal();
			if (Distance < MinDistance)
			{
				FVector Direction = ActualTargetLocation - DampedTargetLocation;
				float Length = Direction.Size();
				if (Length < 0.01F)
				{
					Direction = FVector::VectorPlaneProject(-CameraForward, WorldUp);
				}
				else
				{
					Direction /= Length;
				}
				LocationOffset = Direction * (MinDistance - Distance);
			}
			PreviousTargetLocation += LocationOffset;
		}
	}
	return LocationOffset;
}

void FCinemachineTracker::ForceCameraLocation(UCinemachineBaseStage* Stage, ECVBindingMode BindingMode, FVector Location, FRotator Orientation, FVector CameraOffsetLocalSpace)
{
	FRotator TargetRotation = BindingMode == ECVBindingMode::LazyFollow ? Orientation : GetReferenceOrientation(Stage, BindingMode, IsValid(Stage) && Stage->IsStageValid() ? Stage->GetVirtualCameraState().ReferenceUp : FVector::UpVector);
	PreviousTargetLocation = Location - TargetRotation.Quaternion() * CameraOffsetLocalSpace;
}
