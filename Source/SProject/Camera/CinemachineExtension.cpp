
#include "CinemachineExtension.h"
#include "CinemachineVirtualCameraComponent.h"

UCinemachineExtension::UCinemachineExtension()
{
}

void UCinemachineExtension::BeginPlay()
{
	Super::BeginPlay();
	ConnectToVCamera(true);
}

void UCinemachineExtension::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ConnectToVCamera(false);
}

UCinemachineVirtualCameraComponent* UCinemachineExtension::GetVirtualCamera()
{
	return Cast<UCinemachineVirtualCameraComponent>(GetAttachParent());
}

void UCinemachineExtension::EnsureStarted()
{
	ConnectToVCamera(true);
}

void UCinemachineExtension::ConnectToVCamera(bool bConnect)
{
    UCinemachineVirtualCameraComponent* VCamera = GetVirtualCamera();
    if(IsValid(VCamera))
    {
        if(bConnect)
        {
            VCamera->AddExtension(this);
        }
        else
        {
            VCamera->RemoveExtension(this);
        }
    }
    else
    {
        ExtraState.Empty();
    }
}

void UCinemachineExtension::InvokePostPipelineStageCallback(UCinemachineVirtualCameraComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime)
{
    PostPipelineStageCallback(VCamera, Stage, State, DeltaTime);
}
