

#include "CinemachineBaseComponent.h"
#include "CinemachineVirtualCameraBaseComponent.h"
#include "CinemachineTargetGroupBaseComponent.h"
#include "CinemachineCoreSubSystem.h"
#include "CinemachineBrainComponent.h"

UCinemachineBaseComponent::UCinemachineBaseComponent()
{
}

UCinemachineVirtualCameraBaseComponent* UCinemachineBaseComponent::GetCinemachineVirtualCameraBaseComponent()
{
	if (!IsValid(VCameraOwner))
	{
		VCameraOwner = Cast<UCinemachineVirtualCameraBaseComponent>(GetAttachParent());
	}
	return VCameraOwner;
}

USceneComponent* UCinemachineBaseComponent::GetFollowTarget()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	return IsValid(VCamera) ? VCamera->ResolveFollow(VCamera->GetFollow()) : nullptr;
}

USceneComponent* UCinemachineBaseComponent::GetLookAtTarget()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	return IsValid(VCamera) ? VCamera->ResolveLookAt(VCamera->GetLookAt()) : nullptr;
}

UCinemachineTargetGroupBaseComponent* UCinemachineBaseComponent::GetFollowTargetGroup()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	return IsValid(VCamera) ? VCamera->GetFollowTargetGroup() : nullptr;
}

FVector UCinemachineBaseComponent::GetFollowTargetLocation()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (!IsValid(VCamera))
	{
		return FVector::ZeroVector;
	}
	UCinemachineVirtualCameraBaseComponent* FollowTargetVCamera = VCamera->GetFollowTargetAsVCamera();
	if (!IsValid(FollowTargetVCamera))
	{
		USceneComponent* FollowTarget = GetFollowTarget();
		return IsValid(FollowTarget) ? FollowTarget->GetComponentLocation() : FVector::ZeroVector;
	}
	return FollowTargetVCamera->GetState().FinalLocation();
}

FRotator UCinemachineBaseComponent::GetFollowTargetRotation()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (!IsValid(VCamera))
	{
		return FRotator::ZeroRotator;
	}
	UCinemachineVirtualCameraBaseComponent* FollowTargetVCamera = VCamera->GetFollowTargetAsVCamera();
	if (!IsValid(FollowTargetVCamera))
	{
		USceneComponent* FollowTarget = GetFollowTarget();
		return IsValid(FollowTarget) ? FollowTarget->GetComponentRotation() : FRotator::ZeroRotator;
	}
	return FollowTargetVCamera->GetState().FinalOrientation();
}

UCinemachineTargetGroupBaseComponent* UCinemachineBaseComponent::GetLookAtTargetGroup()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	return IsValid(VCamera) ? VCamera->GetLookAtTargetGroup() : nullptr;
}

FVector UCinemachineBaseComponent::GetLookAtTargetLocation()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (!IsValid(VCamera))
	{
		return FVector::ZeroVector;
	}
	UCinemachineVirtualCameraBaseComponent* LookAtTargetVCamera = VCamera->GetLookAtTargetAsVCamera();
	if (!IsValid(LookAtTargetVCamera))
	{
		USceneComponent* LookAtTarget = GetLookAtTarget();
		return IsValid(LookAtTarget) ? LookAtTarget->GetComponentLocation() : FVector::ZeroVector;
	}
	return LookAtTargetVCamera->GetState().FinalLocation();
}

FRotator UCinemachineBaseComponent::GetLookAtTargetRotation()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (!IsValid(VCamera))
	{
		return FRotator::ZeroRotator;
	}
	UCinemachineVirtualCameraBaseComponent* LookAtTargetVCamera = VCamera->GetLookAtTargetAsVCamera();
	if (!IsValid(LookAtTargetVCamera))
	{
		USceneComponent* LookAtTarget = GetLookAtTarget();
		return IsValid(LookAtTarget) ? LookAtTarget->GetComponentRotation() : FRotator::ZeroRotator;
	}
	return LookAtTargetVCamera->GetState().FinalOrientation();
}

FCinemachineCameraState UCinemachineBaseComponent::GetVirtualCameraState()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	return IsValid(VCamera) ? VCamera->GetState() : FCinemachineCameraState();
}

void UCinemachineBaseComponent::PrePipelineMutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	// Abstract function
}

ECinemachineStage UCinemachineBaseComponent::GetStage() const
{
	return ECinemachineStage::Body;
}

bool UCinemachineBaseComponent::BodyAppliesAfterAim() const
{
	return false;
}

void UCinemachineBaseComponent::MutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	// Abstract function
}

bool UCinemachineBaseComponent::OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime, FCinemachineTransitionParameters& TransitionParameters)
{
	return false;
}

void UCinemachineBaseComponent::OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta)
{
	// Abstract function
}

void UCinemachineBaseComponent::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	// Abstract function
}

float UCinemachineBaseComponent::GetMaxDampTime() const
{
	return 0.0f;
}

bool UCinemachineBaseComponent::RequriesUserInput() const
{
	return false;
}


