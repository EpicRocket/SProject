
#pragma once

#include "Camera/CameraActor.h"
#include "SProjectCamera.generated.h"

UCLASS()
class ASProjectCamera : public ACameraActor
{
   	GENERATED_UCLASS_BODY()

public:
	// Actor interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//~Actor interface

public:
	UPROPERTY(EditDefaultsOnly)
	float ScrollSpeed = 100.0F;

	UPROPERTY(EditDefaultsOnly)
	float Margin = 15.0F;

	int32 ScreenSizeX = 0;

	int32 ScreenSizeY = 0;
};
