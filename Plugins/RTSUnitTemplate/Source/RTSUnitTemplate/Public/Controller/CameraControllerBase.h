// Copyright 2022 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Controller/ControllerBase.h"
#include "Characters/SpeakingUnit.h"
#include "Controller/Input/EnhancedInputComponentBase.h"
#include "CameraControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class RTSUNITTEMPLATE_API ACameraControllerBase : public AControllerBase
{
	GENERATED_BODY()
	
	ACameraControllerBase();

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CheckSpeakingUnits", Keywords = "TopDownRTSCamLib CheckSpeakingUnits"), Category = RTSUnitTemplate)
		bool CheckSpeakingUnits();
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetCameraState", Keywords = "TopDownRTSCamLib SetCameraState"), Category = RTSUnitTemplate)
		void SetCameraState(TEnumAsByte<CameraData::CameraState> NewCameraState);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetViewPortScreenSizes", Keywords = "TopDownRTSCamLib GetViewPortScreenSizes"), Category = RTSUnitTemplate)
	void GetViewPortScreenSizes(int x);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetCameraPanDirection", Keywords = "TopDownRTSCamLib GetCameraPanDirection"), Category = RTSUnitTemplate)
	FVector GetCameraPanDirection();
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CameraBaseMachine", Keywords = "TopDownRTSCamLib CameraBaseMachine"), Category = RTSUnitTemplate)
	void CameraBaseMachine(float DeltaTime);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ToggleLockCamToCharacter", Keywords = "TopDownRTSCamLib ToggleLockCamToCharacter"), Category = RTSUnitTemplate)
	void ToggleLockCamToCharacter();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UnlockCamFromCharacter", Keywords = "TopDownRTSCamLib UnlockCamFromCharacter"), Category = RTSUnitTemplate)
	void UnlockCamFromCharacter();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "LockCamToSpecificUnit", Keywords = "TopDownRTSCamLib LockCamToSpecificUnit"), Category = RTSUnitTemplate)
	void LockCamToSpecificUnit(AUnitBase* SUnit);

	ASpeakingUnit* SpeakingUnit;
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "LockCamToCharacter", Keywords = "TopDownRTSCamLib LockCamToCharacter"), Category = RTSUnitTemplate)
	void LockCamToCharacter(int Index);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "LockCamToCharacter", Keywords = "TopDownRTSCamLib LockCamToCharacter"), Category = RTSUnitTemplate)
	void LockZDistanceToCharacter();
	
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "LockCameraToCharacter", Keywords = "TopDownRTSCamLib LockCameraToCharacter"), Category = RTSUnitTemplate)
	bool LockCameraToCharacter = false;
	
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "CamIsRotatingRight", Keywords = "TopDownRTSCamLib CamIsRotatingRight"), Category = RTSUnitTemplate)
	bool CamIsRotatingRight = false;

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "CamIsRotatingLeft", Keywords = "TopDownRTSCamLib CamIsRotatingLeft"), Category = RTSUnitTemplate)
	bool CamIsRotatingLeft = false;

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "CamIsZoomingIn", Keywords = "TopDownRTSCamLib CamIsZoomingIn"), Category = RTSUnitTemplate)
	bool CamIsZoomingIn = false;

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "CamIsZoomingOut", Keywords = "TopDownRTSCamLib CamIsZoomingOut"), Category = RTSUnitTemplate)
	bool CamIsZoomingOut= false;

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "ZoomOutToPosition", Keywords = "TopDownRTSCamLib ZoomOutToPosition"), Category = RTSUnitTemplate)
	bool ZoomOutToPosition = false;

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "ZoomInToPosition", Keywords = "TopDownRTSCamLib ZoomInToPosition"), Category = RTSUnitTemplate)
	bool ZoomInToPosition = false;

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "HoldZoomOnLockedCharacter", Keywords = "TopDownRTSCamLib HoldZoomOnLockedCharacter"), Category = RTSUnitTemplate)
	bool HoldZoomOnLockedCharacter = false;
	
};