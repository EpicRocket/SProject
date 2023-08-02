

#include "CinemachineExtension.h"
#include "CinemachineVirtualCameraBaseComponent.h"

void UCinemachineExtension::Init(UCinemachineVirtualCameraBaseComponent* InOwner)
{
	if (bIsInitialized)
	{
		return;
	}
	bIsInitialized = true;

	Owner = InOwner;
	OnInitialize();
	bExtensionEnable = false;
	SetEnable(true);
}

void UCinemachineExtension::InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStageType Stage, FCinemachineCameraState& State, float DeltaTime)
{
    PostPipelineStageCallback(VCamera, Stage, State, DeltaTime);
}

void UCinemachineExtension::SetEnable(bool bEnable)
{
	if (bExtensionEnable == bEnable)
	{
		return;
	}

	if (bExtensionEnable == true)
	{
		OnEnable();
	}
	else
	{
		OnDisable();
	}
	ExtraState.Empty();
}

void UCinemachineExtension::OnEnable()
{
	if (IsValid(Owner))
	{
		Owner->AddExtension(this);
	}
}

void UCinemachineExtension::OnDisable()
{
	if (IsValid(Owner))
	{
		Owner->RemoveExtension(this);
	}
}
