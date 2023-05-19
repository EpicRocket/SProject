
#include "CinemachineVirtualCameraComponent.h"

UCinemachineVirtualCameraComponent::UCinemachineVirtualCameraComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCinemachineVirtualCameraComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCinemachineVirtualCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

AActor* UCinemachineVirtualCameraComponent::ResolveFollow(AActor* LocalFollow)
{
    return !IsValid(LocalFollow) && IsValid(ParentCamera) ? ParentCamera->GetFollow() : LocalFollow;
}

AActor* UCinemachineVirtualCameraComponent::ResolveLookAt(AActor* LocalLookAt)
{
    return !IsValid(LocalLookAt) && IsValid(ParentCamera) ? ParentCamera->GetLookAt() : LocalLookAt;
}

void UCinemachineVirtualCameraComponent::UpdateSlaveStatus()
{
    SlaveStatusUpdated = true;
    ParentCamera = Cast<UCinemachineVirtualCameraComponent>(GetAttachParent());
}
