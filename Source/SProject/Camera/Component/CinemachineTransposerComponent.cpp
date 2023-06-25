

#include "CinemachineTransposerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CinemachineCoreSubSystem.h"
#include "Camera/CinemachineVirtualCameraBaseComponent.h"
#include "Shared/VectorExtension.h"

UCinemachineTransposerComponent::UCinemachineTransposerComponent()
	: BindingMode(ECineamchineTransposerBindingMode::LockToTargetWithWorldUp)
	, FollowOffset(FVector::BackwardVector * 10.0f)
	, XDamping(1.0F)
	, YDamping(1.0F)
	, ZDamping(1.0F)
	, PitchDamping(0.0F)
	, YawDamping(0.0F)
	, RollDamping(0.0F)
	, AngularDamping(0.0F)
{
	PrimaryComponentTick.bCanEverTick = true;
}

ECinemachineStage UCinemachineTransposerComponent::GetStage() const
{
	return ECinemachineStage::Body;
}

void UCinemachineTransposerComponent::MutateCameraState(FCinemachineCameraState& State, float DeltaTime)
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

void UCinemachineTransposerComponent::OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta)
{
	if (Target == GetFollowTarget())
	{
		PreviousTargetLocation += LocationDelta;
	}
}

void UCinemachineTransposerComponent::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (IsValid(VCamera))
	{
		FRotator TaregtRotation = BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp ? Rotation : GetReferenceOrientation(VCamera->GetState().ReferenceUp);
		PreviousTargetLocation = Location - TaregtRotation.Quaternion() * GetEffectiveOffset();
	}
}

float UCinemachineTransposerComponent::GetMaxDampTime() const
{
	return FMath::Max(GetDamping().GetMax(), GetAngularDamping().GetMax());
}

FVector UCinemachineTransposerComponent::GetEffectiveOffset() const
{
	FVector Offset = FollowOffset;
	if (BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
	{
		Offset.X = -FMath::Abs(Offset.X);
		Offset.Y = 0.0F;
	}
	return Offset;
}

FRotator UCinemachineTransposerComponent::GetReferenceOrientation(FVector WorldUp)
{
	if (BindingMode == ECineamchineTransposerBindingMode::WorldSpace)
	{
		return FRotator::ZeroRotator;
	}

	USceneComponent* FollowTarget = GetFollowTarget();
	if (IsValid(GetFollowTarget()))
	{
		FRotator TargetOrientation = FollowTarget->GetComponentRotation();
		switch (BindingMode)
		{
		case ECineamchineTransposerBindingMode::LockToTargetOnAssign:
			return TargetOrientationOnAssign;

		case ECineamchineTransposerBindingMode::LockToTargetWithWorldUp:
		{
			FVector Forward = (TargetOrientation.Quaternion() * FVector::ForwardVector).ProjectOnToNormal(WorldUp);
			if (Forward.IsNearlyZero())
			{
				break;
			}
			return UVectorExtension::LookRotation(Forward, WorldUp);
		}

		case ECineamchineTransposerBindingMode::LockToTargetNoRoll:
			return UVectorExtension::LookRotation(TargetOrientation.Quaternion() * FVector::ForwardVector, WorldUp);

		case ECineamchineTransposerBindingMode::LockToTarget:
			return TargetOrientation;

		case ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp:
		{
			FVector Forward = (GetFollowTargetLocation() - GetVirtualCameraState().RawLocation).ProjectOnToNormal(WorldUp);
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

FVector UCinemachineTransposerComponent::GetTargetCameraLocation(FVector WorldUp)
{
	if (!IsValid(GetFollowTarget()))
	{
		return FVector::ZeroVector;
	}
	return GetFollowTargetLocation() + GetReferenceOrientation(WorldUp).Quaternion() * GetEffectiveOffset();
}

void UCinemachineTransposerComponent::InitPrevFrameStateInfo(FCinemachineCameraState& State, float DeltaTime)
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	bool bPrevStateValid = DeltaTime > 0.0F && IsValid(VCamera) && VCamera->GetPreviousStateIsValid();

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

void UCinemachineTransposerComponent::TrackTarget(float DeltaTime, FVector Up, FVector DesiredCameraOffset, FVector& OutTargetLocation, FRotator& OutTargetOrient)
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (!IsValid(VCamera))
	{
		return;
	}

	FRotator TargetOrientation = GetReferenceOrientation(Up);
	FRotator DampedOrientation = TargetOrientation;
	bool bPrevStateValid = DeltaTime >= 0.0F && VCamera->GetPreviousStateIsValid();
	if (bPrevStateValid)
	{
		if (BindingMode == ECineamchineTransposerBindingMode::LockToTarget)
		{
			float Alpha = VCamera->DetachedFollowTargetDamp(1.0F, AngularDamping, DeltaTime);
			DampedOrientation = FMath::RInterpTo(PreviousReferenceOrientation, TargetOrientation, Alpha, 1.0F);
		}
		else if (BindingMode != ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
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
			Relative = VCamera->DetachedFollowTargetDamp(Relative, AngularDamping, DeltaTime);
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
		FQuat Q = UVectorExtension::SafeFromToRotation(PreviousOffset.ProjectOnToNormal(Up), DesiredCameraOffset.ProjectOnToNormal(Up), Up);
		CurrentLocation = TargetLocation + Q * (PreviousTargetLocation - TargetLocation);
	}
	PreviousOffset = DesiredCameraOffset;

	FVector LocationDelta = TargetLocation - CurrentLocation;
	if (bPrevStateValid)
	{
		FRotator DampingSpace = DesiredCameraOffset.IsNearlyZero() ? GetVirtualCameraState().RawOrientation : UVectorExtension::LookRotation(DampedOrientation.Quaternion() * DesiredCameraOffset, Up);
		FVector LocalDelta = DampingSpace.GetInverse().Quaternion() * LocationDelta;
		LocalDelta = VCamera->DetachedFollowTargetDamp(LocalDelta, GetDamping(), DeltaTime);
		LocationDelta = DampingSpace.Quaternion() * LocalDelta;
	}
	CurrentLocation += LocationDelta;

	OutTargetLocation = PreviousTargetLocation = CurrentLocation;
	OutTargetOrient = DampedOrientation;
}

FVector UCinemachineTransposerComponent::GetOffsetForMinimumTargetDistance(FVector DampedTargetLocation, FVector CameraOffset, FVector CameraForward, FVector Up, FVector ActualTargetLocation)
{
	FVector LocationOffset = FVector::ZeroVector;

	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (!IsValid(VCamera))
	{
		return LocationOffset;
	}

	if (VCamera->FollowTargetAttachment > (1 - UE_KINDA_SMALL_NUMBER))
	{
		CameraOffset = CameraOffset.ProjectOnToNormal(Up);
		float MinDistance = CameraOffset.SizeSquared() * 0.2F;
		if (MinDistance > 0.0F)
		{
			ActualTargetLocation = ActualTargetLocation.ProjectOnToNormal(Up);
			DampedTargetLocation = DampedTargetLocation.ProjectOnToNormal(Up);
			FVector CamerLocation = DampedTargetLocation + CameraOffset;
			float Distance = (ActualTargetLocation - CamerLocation) | (DampedTargetLocation - CamerLocation);
			if (Distance < MinDistance)
			{
				FVector Direction = ActualTargetLocation - DampedTargetLocation;
				float Length = Direction.SizeSquared();
				if (Length < 0.01F)
				{
					Direction = -CameraForward.ProjectOnToNormal(Up);
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

FVector UCinemachineTransposerComponent::GetDamping() const
{
	if (BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
	{
		return FVector(0.0F, YDamping, ZDamping);
	}
	else
	{
		return FVector(XDamping, YDamping, ZDamping);
	}
}

FVector UCinemachineTransposerComponent::GetAngularDamping() const
{
	switch (BindingMode)
	{
	case ECineamchineTransposerBindingMode::LockToTargetOnAssign: // falls through
	case ECineamchineTransposerBindingMode::WorldSpace:			  // falls through
	case ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp:
	{
		return FVector::ZeroVector;
	}

	case ECineamchineTransposerBindingMode::LockToTargetWithWorldUp:
		return FVector(0.0F, YawDamping, 0.0F);

	case ECineamchineTransposerBindingMode::LockToTargetNoRoll:
		return FVector(PitchDamping, YawDamping, 0.0F);

	default:
		return FVector(PitchDamping, YawDamping, RollDamping);
	}
}
