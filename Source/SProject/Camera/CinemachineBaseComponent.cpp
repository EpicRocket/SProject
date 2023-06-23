
#include "CinemachineBaseComponent.h"
#include "GameFramework/Actor.h"
#include "CinemachineVirtualCameraComponent.h"

AActor* UCinemachineBaseComponent::GetFollowTarget()
{
    if (CameraComponent == nullptr)
    {
        return nullptr;
    }
	return CameraComponent->ResolveFollow(CameraComponent->GetFollow());
}

AActor* UCinemachineBaseComponent::GetLookAtTarget()
{
    if (CameraComponent == nullptr)
    {
        return nullptr;
    }
    return CameraComponent->ResolveLookAt(CameraComponent->GetLookAt());
}
