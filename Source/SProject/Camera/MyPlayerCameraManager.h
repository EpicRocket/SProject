

#pragma once

#include "Camera/PlayerCameraManager.h"

#include "MyPlayerCameraManager.generated.h"

class FDebugDisplayInfo;
class UCanvas;
class UObject;


#define My_CAMERA_DEFAULT_FOV			(80.0f)
#define My_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define My_CAMERA_DEFAULT_PITCH_MAX	(89.0f)

class UMyUICameraManagerComponent;

/**
 * AMyPlayerCameraManager
 *
 *	The base player camera manager class used by this project.
 */
UCLASS(notplaceable, MinimalAPI)
class AMyPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	AMyPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	UMyUICameraManagerComponent* GetUICameraComponent() const;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	/** The UI Camera Component, controls the camera when UI is doing something important that gameplay doesn't get priority over. */
	UPROPERTY(Transient)
	TObjectPtr<UMyUICameraManagerComponent> UICamera;
};
