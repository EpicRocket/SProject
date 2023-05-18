
#include "CinemachineLensSettings.h"
#include "Camera/CameraComponent.h"

UCinemachineLensSettings UCinemachineLensSettings::Default;

UCinemachineLensSettings UCinemachineLensSettings::FromCamera(UCameraComponent* CameraComponent)
{
	//UCinemachineLensSettings LensSettings = UCinemachineLensSettings::Default;
    /*if(IsValid(CameraComponent))
    {
		LensSettings.FieldOfView = CameraComponent->FieldOfView;
		LensSettings.OrthographicSize = CameraComponent->OrthoWidth;
		LensSettings.NearClipPlane = CameraComponent->OrthoNearClipPlane;
		LensSettings.FarClipPlane = CameraComponent->OrthoFarClipPlane;
		LensSettings.SnapshotCameraReadOnlyProperties(CameraComponent);
    }*/
    return UCinemachineLensSettings();
}

void UCinemachineLensSettings::SnapshotCameraReadOnlyProperties(UCameraComponent* CameraComponent)
{
	bOrthographic = false;
	bPhysicalCamera = false;
	if (IsValid(CameraComponent) && ModeOverride == ECinemachineLensSettginsOverrideModes::None)
	{
		bOrthographic = CameraComponent->ProjectionMode == ECameraProjectionMode::Orthographic;
		bPhysicalCamera = CameraComponent->bUseFieldOfViewForLOD;
	}
}

