

#include "CinemachineExtension.h"
#include "CinemachineVirtualCameraBaseComponent.h"

UCinemachineExtension::UCinemachineExtension()
	: bExtensionEnable(true)
{
	bCacheExtensionEnable = bExtensionEnable;
}

void UCinemachineExtension::BeginPlay()
{
	Super::BeginPlay();
	if (true == bExtensionEnable)
	{
		bCacheExtensionEnable = !bExtensionEnable;
		SetEnable(true);
	}
	else
	{
		bCacheExtensionEnable = bExtensionEnable;
	}
}

void UCinemachineExtension::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SetEnable(false);
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UCinemachineExtension::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld())
	{
		SetEnable(bExtensionEnable);
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

UCinemachineVirtualCameraBaseComponent* UCinemachineExtension::GetVirtualCamera()
{
	return Cast<UCinemachineVirtualCameraBaseComponent>(GetAttachParent());
}

void UCinemachineExtension::SetEnable(bool bEnable)
{
	if (bCacheExtensionEnable == bEnable)
	{
		return;
	}
	bExtensionEnable = bEnable;

	if (bEnable)
	{
		SetComponentTickEnabled(true);
		OnEnable();
	}
	else
	{
		OnDisable();
		SetComponentTickEnabled(false);
	}

	bCacheExtensionEnable = bExtensionEnable;
}

void UCinemachineExtension::ConnectToVCamera(bool bConnect)
{
    UCinemachineVirtualCameraBaseComponent* VCamera = GetVirtualCamera();
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

void UCinemachineExtension::OnEnable()
{
	ConnectToVCamera(true);
}

void UCinemachineExtension::OnDisable()
{
	ConnectToVCamera(false);
}

void UCinemachineExtension::InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime)
{
    PostPipelineStageCallback(VCamera, Stage, State, DeltaTime);
}
