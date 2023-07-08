

#include "CinemachineBaseStage.h"
#include "CinemachineVirtualCameraBaseComponent.h"
#include "CinemachineTargetGroupBaseComponent.h"
#include "CinemachineCoreSubSystem.h"
#include "CinemachineBrainComponent.h"

UCinemachineBaseStage::UCinemachineBaseStage()
{
}

USceneComponent* UCinemachineBaseStage::GetFollowTarget()
{
	return IsValid(Owner) ? Owner->ResolveFollow(Owner->GetFollow()) : nullptr;
}

USceneComponent* UCinemachineBaseStage::GetLookAtTarget()
{
	return IsValid(Owner) ? Owner->ResolveLookAt(Owner->GetLookAt()) : nullptr;
}

UCinemachineTargetGroupBaseComponent* UCinemachineBaseStage::GetFollowTargetGroup()
{
	return IsValid(Owner) ? Owner->GetFollowTargetGroup() : nullptr;
}

FVector UCinemachineBaseStage::GetFollowTargetLocation()
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
	return FollowTargetVCamera->GetState().FinalLocation();
}

FRotator UCinemachineBaseStage::GetFollowTargetRotation()
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
	return FollowTargetVCamera->GetState().FinalOrientation();
}

UCinemachineTargetGroupBaseComponent* UCinemachineBaseStage::GetLookAtTargetGroup()
{
	return IsValid(Owner) ? Owner->GetLookAtTargetGroup() : nullptr;
}

FVector UCinemachineBaseStage::GetLookAtTargetLocation()
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
	return LookAtTargetVCamera->GetState().FinalLocation();
}

FRotator UCinemachineBaseStage::GetLookAtTargetRotation()
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
	return LookAtTargetVCamera->GetState().FinalOrientation();
}

FCinemachineCameraState UCinemachineBaseStage::GetVirtualCameraState()
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

void UCinemachineBaseStage::PrePipelineMutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	// Abstract function
}

ECVStage UCinemachineBaseStage::GetStage() const
{
	return ECVStage::Body;
}

bool UCinemachineBaseStage::BodyAppliesAfterAim() const
{
	return false;
}

void UCinemachineBaseStage::MutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	// Abstract function
}

bool UCinemachineBaseStage::OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime, FCinemachineTransitionParameters& TransitionParameters)
{
	return false;
}

void UCinemachineBaseStage::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	// Abstract function
}

float UCinemachineBaseStage::GetMaxDampTime() const
{
	return 0.0f;
}

bool UCinemachineBaseStage::RequiresUserInput() const
{
	return false;
}
