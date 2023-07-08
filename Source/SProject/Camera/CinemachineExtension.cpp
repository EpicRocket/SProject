

#include "CinemachineExtension.h"
#include "CinemachineVirtualCameraBaseComponent.h"

void UCinemachineExtension::ConnectToVCamera(bool bConnect)
{
    if(IsValid(Owner))
    {
        if(bConnect)
        {
			Owner->AddExtension(this);
        }
        else
        {
			Owner->RemoveExtension(this);
        }
    }
    else
    {
        ExtraState.Empty();
    }
}

void UCinemachineExtension::Init(UCinemachineVirtualCameraBaseComponent* OwnerVirtualCamera)
{
	if (bIsInitialized)
	{
		return;
	}
	bIsInitialized = true;

	Owner = OwnerVirtualCamera;
	OnInitialize();
	ConnectToVCamera(true);
}

void UCinemachineExtension::InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStage Stage, FCinemachineCameraState& State, float DeltaTime)
{
    PostPipelineStageCallback(VCamera, Stage, State, DeltaTime);
}
