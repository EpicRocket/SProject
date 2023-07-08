

#include "CinemachineTransposer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CinemachineCoreSubSystem.h"
#include "Camera/CinemachineVirtualCameraBaseComponent.h"
#include "Shared/VectorExtension.h"

ECVStage UCinemachineTransposer::GetStage() const
{
	return ECVStage::Body;
}

void UCinemachineTransposer::MutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	InitPrevFrameStateInfo(State, DeltaTime);
	if (!IsValid(GetFollowTarget()))
	{
		return;
	}
	FVector Offset = GetEffectiveOffset();
	FVector Location = FVector::ZeroVector;
	FRotator Orient = FRotator::ZeroRotator;
	TrackTarget(DeltaTime, State.ReferenceUp, Offset, Location, Orient);
	Offset = Orient.Quaternion() * Offset;

	State.ReferenceUp = Orient.Quaternion() * FVector::UpVector;

	FVector TargetLocation = GetFollowTargetLocation();
	Location += GetOffsetForMinimumTargetDistance(Location, Offset, State.RawOrientation.Quaternion() * FVector::ForwardVector, State.ReferenceUp, TargetLocation);

	State.RawLocation = Location + Offset;
}

void UCinemachineTransposer::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	if (IsValid(Owner))
	{
		FRotator TaregtRotation = TransposerData.BindingMode == ECVBindingMode::SimpleFollowWithWorldUp ? Rotation : GetReferenceOrientation(Owner->GetState().ReferenceUp);
		PreviousTargetLocation = Location - TaregtRotation.Quaternion() * GetEffectiveOffset();
	}
}

float UCinemachineTransposer::GetMaxDampTime() const
{
	return FMath::Max(GetDamping().GetMax(), GetAngularDamping().GetMax());
}

FVector UCinemachineTransposer::GetEffectiveOffset() const
{
	FVector Offset = TransposerData.FollowOffset;
	if (TransposerData.BindingMode == ECVBindingMode::SimpleFollowWithWorldUp)
	{
		Offset.X = -FMath::Abs(Offset.X);
		Offset.Y = 0.0F;
	}
	return Offset;
}

FRotator UCinemachineTransposer::GetReferenceOrientation(FVector WorldUp)
{
	if (TransposerData.BindingMode == ECVBindingMode::WorldSpace)
	{
		return FRotator::ZeroRotator;
	}

	USceneComponent* FollowTarget = GetFollowTarget();
	if (IsValid(GetFollowTarget()))
	{
		FRotator TargetOrientation = FollowTarget->GetComponentRotation();
		switch (TransposerData.BindingMode)
		{
		case ECVBindingMode::LockToTargetOnAssign:
			return TargetOrientationOnAssign;

		case ECVBindingMode::LockToTargetWithWorldUp:
		{
			FVector Forward = FVector::VectorPlaneProject(TargetOrientation.Quaternion() * FVector::ForwardVector, WorldUp);
			if (Forward.IsNearlyZero())
			{
				break;
			}
			return UVectorExtension::LookRotation(Forward, WorldUp);
		}

		case ECVBindingMode::LockToTargetNoRoll:
			return UVectorExtension::LookRotation(TargetOrientation.Quaternion() * FVector::ForwardVector, WorldUp);

		case ECVBindingMode::LockToTarget:
			return TargetOrientation;

		case ECVBindingMode::SimpleFollowWithWorldUp:
		{
			FVector Forward = FVector::VectorPlaneProject(GetFollowTargetLocation() - GetVirtualCameraState().RawLocation, WorldUp);
			if (Forward.IsNearlyZero())
			{
				break;
			}
			return UVectorExtension::LookRotation(Forward, WorldUp);
		}
		}
	}
	return PreviousReferenceOrientation.GetNormalized();
}

FVector UCinemachineTransposer::GetTargetCameraLocation(FVector WorldUp)
{
	if (!IsValid(GetFollowTarget()))
	{
		return FVector::ZeroVector;
	}
	return GetFollowTargetLocation() + GetReferenceOrientation(WorldUp).Quaternion() * GetEffectiveOffset();
}

void UCinemachineTransposer::InitPrevFrameStateInfo(FCinemachineCameraState& State, float DeltaTime)
{
	bool bPrevStateValid = DeltaTime > 0.0F && IsValid(Owner) && Owner->GetPreviousStateIsValid();

	USceneComponent* FollowTarget = GetFollowTarget();

	if (PreviousTarget != FollowTarget || !bPrevStateValid)
	{
		PreviousTarget = FollowTarget;
		TargetOrientationOnAssign = GetFollowTargetRotation();
	}
	if (!bPrevStateValid)
	{
		PreviousTargetLocation = GetFollowTargetLocation();
		TargetOrientationOnAssign = GetReferenceOrientation(State.ReferenceUp);
	}
}

void UCinemachineTransposer::TrackTarget(float DeltaTime, FVector Up, FVector DesiredCameraOffset, FVector& OutTargetLocation, FRotator& OutTargetOrient)
{
	if (!IsValid(Owner))
	{
		return;
	}

	FRotator TargetOrientation = GetReferenceOrientation(Up);
	FRotator DampedOrientation = TargetOrientation;
	bool bPrevStateValid = DeltaTime >= 0.0F && Owner->GetPreviousStateIsValid();
	if (bPrevStateValid)
	{
		if (TransposerData.BindingMode == ECVBindingMode::LockToTarget)
		{
			float Alpha = Owner->DetachedFollowTargetDamp(1.0F, TransposerData.AngularDamping, DeltaTime);
			DampedOrientation = FMath::RInterpTo(PreviousReferenceOrientation, TargetOrientation, Alpha, 1.0F);
		}
		else if (TransposerData.BindingMode != ECVBindingMode::SimpleFollowWithWorldUp)
		{
			FVector Relative = (PreviousReferenceOrientation.GetInverse().Quaternion() * TargetOrientation.Quaternion()).Euler();
			for (int32 i = 0; i < 3; ++i)
			{
				if (Relative[i] > 180.0F)
				{
					Relative[i] -= 360.0F;
				}
				if (FMath::Abs(Relative[i]) < 0.01F)
				{
					Relative[i] = 0.0F;
				}
			}
			Relative = Owner->DetachedFollowTargetDamp(Relative, TransposerData.AngularDamping, DeltaTime);
			DampedOrientation = (PreviousReferenceOrientation.Quaternion() * FQuat::MakeFromEuler(Relative)).Rotator();
		}
	}
	PreviousReferenceOrientation = DampedOrientation;

	FVector TargetLocation = GetFollowTargetLocation();
	FVector CurrentLocation = PreviousTargetLocation;
	FVector LocalPreviousOffset = bPrevStateValid ? PreviousOffset : DesiredCameraOffset;
	FVector OffsetDelta = DesiredCameraOffset - LocalPreviousOffset;
	if (!OffsetDelta.IsNearlyZero())
	{
		FQuat Q = UVectorExtension::SafeFromToRotation(FVector::VectorPlaneProject(PreviousOffset, Up), FVector::VectorPlaneProject(DesiredCameraOffset, Up), Up);
		CurrentLocation = TargetLocation + Q * (PreviousTargetLocation - TargetLocation);
	}
	PreviousOffset = DesiredCameraOffset;

	FVector LocationDelta = TargetLocation - CurrentLocation;
	if (bPrevStateValid)
	{
		FRotator DampingSpace = DesiredCameraOffset.IsNearlyZero() ? GetVirtualCameraState().RawOrientation : UVectorExtension::LookRotation(DampedOrientation.Quaternion() * DesiredCameraOffset, Up);
		FVector LocalDelta = DampingSpace.GetInverse().Quaternion() * LocationDelta;
		LocalDelta = Owner->DetachedFollowTargetDamp(LocalDelta, GetDamping(), DeltaTime);
		LocationDelta = DampingSpace.Quaternion() * LocalDelta;
	}
	CurrentLocation += LocationDelta;

	OutTargetLocation = PreviousTargetLocation = CurrentLocation;
	OutTargetOrient = DampedOrientation;
}

FVector UCinemachineTransposer::GetOffsetForMinimumTargetDistance(FVector DampedTargetLocation, FVector CameraOffset, FVector CameraForward, FVector Up, FVector ActualTargetLocation)
{
	FVector LocationOffset = FVector::ZeroVector;
	if (!IsValid(Owner))
	{
		return LocationOffset;
	}

	if (Owner->FollowTargetAttachment > (1 - UE_KINDA_SMALL_NUMBER))
	{
		CameraOffset = FVector::VectorPlaneProject(CameraOffset, Up);
		float MinDistance = CameraOffset.SizeSquared() * 0.2F;
		if (MinDistance > 0.0F)
		{
			ActualTargetLocation = FVector::VectorPlaneProject(ActualTargetLocation, Up);
			DampedTargetLocation = FVector::VectorPlaneProject(DampedTargetLocation, Up);
			FVector CamerLocation = DampedTargetLocation + CameraOffset;
			float Distance = (ActualTargetLocation - CamerLocation) | (DampedTargetLocation - CamerLocation);
			if (Distance < MinDistance)
			{
				FVector Direction = ActualTargetLocation - DampedTargetLocation;
				float Length = Direction.SizeSquared();
				if (Length < 0.01F)
				{
					Direction = FVector::VectorPlaneProject(-CameraForward, Up);
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

FVector UCinemachineTransposer::GetDamping() const
{
	if (TransposerData.BindingMode == ECVBindingMode::SimpleFollowWithWorldUp)
	{
		return FVector(0.0F, TransposerData.YDamping, TransposerData.ZDamping);
	}
	else
	{
		return FVector(TransposerData.XDamping, TransposerData.YDamping, TransposerData.ZDamping);
	}
}

FVector UCinemachineTransposer::GetAngularDamping() const
{
	switch (TransposerData.BindingMode)
	{
	case ECVBindingMode::LockToTargetOnAssign: // falls through
	case ECVBindingMode::WorldSpace:		   // falls through
	case ECVBindingMode::SimpleFollowWithWorldUp:
	{
		return FVector::ZeroVector;
	}

	case ECVBindingMode::LockToTargetWithWorldUp:
		return FVector(0.0F, TransposerData.YawDamping, 0.0F);

	case ECVBindingMode::LockToTargetNoRoll:
		return FVector(TransposerData.PitchDamping, TransposerData.YawDamping, 0.0F);

	default:
		return FVector(TransposerData.PitchDamping, TransposerData.YawDamping, TransposerData.RollDamping);
	}
}
