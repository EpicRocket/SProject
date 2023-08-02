

#include "CinemachineBaseStage.h"
#include "CinemachineVirtualCameraBaseComponent.h"
#include "CinemachineTargetGroupComponent.h"
#include "CinemachineCoreSubSystem.h"
#include "CinemachineBrainComponent.h"

UCinemachineBaseStage::UCinemachineBaseStage()
{
}

USceneComponent* UCinemachineBaseStage::GetFollowTarget() const
{
	return IsValid(Owner) ? Owner->ResolveFollow(Owner->GetFollow()) : nullptr;
}

USceneComponent* UCinemachineBaseStage::GetLookAtTarget() const
{
	return IsValid(Owner) ? Owner->ResolveLookAt(Owner->GetLookAt()) : nullptr;
}

UCinemachineTargetGroupComponent* UCinemachineBaseStage::GetFollowTargetGroup() const
{
	return IsValid(Owner) ? Owner->GetFollowTargetGroup() : nullptr;
}

FVector UCinemachineBaseStage::GetFollowTargetLocation() const
{
	if (!IsValid(Owner))
	{
		return FVector::ZeroVector;
	}
	UCinemachineVirtualCameraBaseComponent* FollowTargetVCamera = Owner->GetFollowTargetAsVCamera();
	if (!IsValid(FollowTargetVCamera))
	{
		USceneComponent* FollowTarget = GetFollowTarget();
		return IsValid(FollowTarget) ? FollowTarget->GetComponentLocation() : FVector::ZeroVector;
	}
	return FollowTargetVCamera->GetState().GetFinalLocation();
}

FRotator UCinemachineBaseStage::GetFollowTargetRotation() const
{
	if (!IsValid(Owner))
	{
		return FRotator::ZeroRotator;
	}
	UCinemachineVirtualCameraBaseComponent* FollowTargetVCamera = Owner->GetFollowTargetAsVCamera();
	if (!IsValid(FollowTargetVCamera))
	{
		USceneComponent* FollowTarget = GetFollowTarget();
		return IsValid(FollowTarget) ? FollowTarget->GetComponentRotation() : FRotator::ZeroRotator;
	}
	return FollowTargetVCamera->GetState().GetFinalOrientation();
}

UCinemachineTargetGroupComponent* UCinemachineBaseStage::GetLookAtTargetGroup() const
{
	return IsValid(Owner) ? Owner->GetLookAtTargetGroup() : nullptr;
}

FVector UCinemachineBaseStage::GetLookAtTargetLocation() const
{
	if (!IsValid(Owner))
	{
		return FVector::ZeroVector;
	}
	UCinemachineVirtualCameraBaseComponent* LookAtTargetVCamera = Owner->GetLookAtTargetAsVCamera();
	if (!IsValid(LookAtTargetVCamera))
	{
		USceneComponent* LookAtTarget = GetLookAtTarget();
		return IsValid(LookAtTarget) ? LookAtTarget->GetComponentLocation() : FVector::ZeroVector;
	}
	return LookAtTargetVCamera->GetState().GetFinalLocation();
}

FRotator UCinemachineBaseStage::GetLookAtTargetRotation() const
{
	if (!IsValid(Owner))
	{
		return FRotator::ZeroRotator;
	}
	UCinemachineVirtualCameraBaseComponent* LookAtTargetVCamera = Owner->GetLookAtTargetAsVCamera();
	if (!IsValid(LookAtTargetVCamera))
	{
		USceneComponent* LookAtTarget = GetLookAtTarget();
		return IsValid(LookAtTarget) ? LookAtTarget->GetComponentRotation() : FRotator::ZeroRotator;
	}
	return LookAtTargetVCamera->GetState().GetFinalOrientation();
}

FCinemachineCameraState UCinemachineBaseStage::GetVirtualCameraState() const
{
	return IsValid(Owner) ? Owner->GetState() : FCinemachineCameraState();
}

void UCinemachineBaseStage::Init(UCinemachineVirtualCameraBaseComponent* OwnerVirtualCamera)
{
	if (bIsInitialized)
	{
		return;
	}
	bIsInitialized = true;

	Owner = OwnerVirtualCamera;
	OnInitialize();
}
