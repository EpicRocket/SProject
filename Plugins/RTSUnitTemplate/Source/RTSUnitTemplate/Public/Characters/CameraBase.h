// Copyright 2022 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Controller/Input/InputConfig.h"
#include "InputActionValue.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Characters/UnitBase.h"
#include "Components/WidgetComponent.h"
#include "Core/UnitData.h"
#include "InputMappingContext.h"
#include "CameraBase.generated.h"


UCLASS()
class RTSUNITTEMPLATE_API ACameraBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACameraBase(const FObjectInitializer& ObjectInitializer);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputConfig* InputConfig;

	/** Handles Enhanced Keyboard Inputs */
	void Input_LeftClick_Pressed(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_LeftClick_Released(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_RightClick_Pressed(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_G_Pressed(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_A_Pressed(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_Ctrl_Pressed(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_Ctrl_Released(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_Tab_Pressed(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_Tab_Released(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_Shift_Pressed(const FInputActionValue& InputActionValue, int32 CamState);
	void Input_Shift_Released(const FInputActionValue& InputActionValue, int32 CamState);
	/** Handles Enhanced Keyboard Inputs */

	
	void SwitchControllerStateMachine(const FInputActionValue& InputActionValue, int32 NewCameraState);

	bool BlockControls = true;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CreateCameraComp", Keywords = "RTSUnitTemplate CreateCameraComp"), Category = RTSUnitTemplate)
		void CreateCameraComp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "RootScene", Keywords = "RTSUnitTemplate RootScene"), Category = RTSUnitTemplate)
		USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "SpringArm", Keywords = "RTSUnitTemplate SpringArm"), Category = RTSUnitTemplate)
		USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "SpringArmRotator", Keywords = "RTSUnitTemplate SpringArmRotator"), Category = RTSUnitTemplate)
		FRotator SpringArmRotator = FRotator(-50, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "CameraComp", Keywords = "RTSUnitTemplate CameraComp"), Category = RTSUnitTemplate)
		UCameraComponent* CameraComp;

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "PC", Keywords = "RTSUnitTemplate PC"), Category = RTSUnitTemplate)
		APlayerController* PC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "MappingContext", Keywords = "TopDownRTSCamLib MappingContext"), Category = TopDownRTSCamLib)
		UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "MappingPriority", Keywords = "TopDownRTSCamLib MappingPriority"), Category = TopDownRTSCamLib)
		int32 MappingPriority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "CameraDistanceToCharacter", Keywords = "TopDownRTSCamLib CameraDistanceToCharacter"), Category = TopDownRTSCamLib)
		float CameraDistanceToCharacter;
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SpawnControllWidget", Keywords = "RTSUnitTemplate SpawnControllWidget"), Category = RTSUnitTemplate)
		void SpawnControllWidget();

	UFUNCTION( BlueprintCallable, meta = (DisplayName = "PanMoveCamera", Keywords = "RTSUnitTemplate PanMoveCamera"), Category = RTSUnitTemplate)
		void PanMoveCamera(const FVector& NewPanDirection);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Margin", Keywords = "RTSUnitTemplate Margin"), Category = RTSUnitTemplate)
		float Margin = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ScreenSizeX", Keywords = "RTSUnitTemplate ScreenSizeX"), Category = RTSUnitTemplate)
		int32 ScreenSizeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ScreenSizeY", Keywords = "RTSUnitTemplate ScreenSizeY"), Category = RTSUnitTemplate)
		int32 ScreenSizeY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "GetViewPortScreenSizesState", Keywords = "RTSUnitTemplate GetViewPortScreenSizesState"), Category = RTSUnitTemplate)
		int GetViewPortScreenSizesState = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "CamSpeed", Keywords = "RTSUnitTemplate CamSpeed"), Category = RTSUnitTemplate)
		float CamSpeed = 80;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ZoomIn", Keywords = "RTSUnitTemplate ZoomIn"), Category = RTSUnitTemplate)
		void ZoomIn();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ZoomOut", Keywords = "RTSUnitTemplate ZoomOut"), Category = RTSUnitTemplate)
		void ZoomOut();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "RotateCamLeft", Keywords = "RTSUnitTemplate RotateCamLeft"), Category = TopDownRTSCamLib)
		bool RotateCamLeft(bool UseCamRotationOffset, float Add); // CamRotationOffset

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "RotateCamRight", Keywords = "RTSUnitTemplate RotateCamRight"), Category = TopDownRTSCamLib)
		bool RotateCamRight(bool UseCamRotationOffset, float Add); // CamRotationOffset
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "RotateCamLeft", Keywords = "RTSUnitTemplate RotateCamLeft"), Category = TopDownRTSCamLib)
		bool RotateCamLeftTo(float Position, float Add);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "RotateCamRight", Keywords = "RTSUnitTemplate RotateCamRight"), Category = TopDownRTSCamLib)
		bool RotateCamRightTo(float Position, float Add);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "JumpCamera", Keywords = "RTSUnitTemplate JumpCamera"), Category = RTSUnitTemplate)
		void JumpCamera(FHitResult Hit);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetMousePos2D", Keywords = "RTSUnitTemplate GetMousePos2D"), Category = RTSUnitTemplate)
		FVector2D GetMousePos2D();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Zoom", Keywords = "RTSUnitTemplate Zoom"), Category = RTSUnitTemplate)
		void Zoom();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ZoomOutToPosition", Keywords = "RTSUnitTemplate ZoomOutToPosition"), Category = RTSUnitTemplate)
		bool ZoomOutToPosition(float Distance, const FVector SelectedActorPosition = FVector(0.f,0.f,0.f));
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ZoomInToPosition", Keywords = "RTSUnitTemplate ZoomInToPosition"), Category = RTSUnitTemplate)
		bool ZoomInToPosition(float Distance, const FVector SelectedActorPosition = FVector(0.f,0.f,0.f));

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "LockOnUnit", Keywords = "RTSUnitTemplate LockOnUnit"), Category = RTSUnitTemplate)
		void LockOnUnit(AUnitBase* Unit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ZoomOutPosition", Keywords = "RTSUnitTemplate ZoomOutPosition"), Category = RTSUnitTemplate)
		float ZoomOutPosition = 20000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ZoomPosition", Keywords = "RTSUnitTemplate ZoomPosition"), Category = RTSUnitTemplate)
		float ZoomPosition = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "PitchValue", Keywords = "RTSUnitTemplate PitchValue"), Category = RTSUnitTemplate)
		float PitchValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "YawValue", Keywords = "RTSUnitTemplate YawValue"), Category = RTSUnitTemplate)
		float YawValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "RollValue", Keywords = "RTSUnitTemplate RollValue"), Category = RTSUnitTemplate)
		float RollValue = 0.f;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ZoomThirdPersonPosition", Keywords = "TopDownRTSCamLib ZoomThirdPersonPosition"), Category = RTSUnitTemplate)
		bool IsCharacterDistanceTooLow(float Distance, const FVector SelectedActorPosition = FVector(0.f,0.f,0.f));

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "IsCharacterDistanceTooHigh", Keywords = "TopDownRTSCamLib IsCharacterDistanceTooHigh"), Category = RTSUnitTemplate)
		bool IsCharacterDistanceTooHigh(float Distance, const FVector SelectedActorPosition);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ZoomThirdPersonPosition", Keywords = "TopDownRTSCamLib ZoomThirdPersonPosition"), Category = RTSUnitTemplate)
		bool ZoomInToThirdPerson(const FVector SelectedActorPosition = FVector(0.f,0.f,0.f));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ZoomThirdPersonPosition", Keywords = "TopDownRTSCamLib ZoomThirdPersonPosition"), Category = RTSUnitTemplate)
		float ZoomThirdPersonPosition = 600.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "CamRotationOffset", Keywords = "TopDownRTSCamLib CamRotationOffset"), Category = RTSUnitTemplate)
		float CamRotationOffset = 11.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "AddCamRotation", Keywords = "TopDownRTSCamLib AddCamRotation"), Category = RTSUnitTemplate)
		float AddCamRotation = 1.0f;
	
	UPROPERTY(EditAnywhere, meta = (DisplayName = "CameraAngles", Keywords = "TopDownRTSCamLib CameraAngles"), Category = RTSUnitTemplate)
		float CameraAngles[4] = { 0.f, 90.f, 180.f, 270.f };

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "RotationDegreeStep", Keywords = "TopDownRTSCamLib RotationDegreeStep"), Category = RTSUnitTemplate)
		float RotationDegreeStep = 90.f;
	
	bool IsCameraInAngle()
	{
		if(YawValue == 360.f) YawValue = 0.f;
		bool IsInAngle = false;
		for( int i = 0; i < 4 ; i++)
		{
			if(YawValue == CameraAngles[i]) IsInAngle = true;
		}
		return IsInAngle;
	}

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MoveCamToForward", Keywords = "TopDownRTSCamLib MoveCamToForward"), Category = RTSUnitTemplate)
		void MoveCamToForward();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MoveCamToBackward", Keywords = "TopDownRTSCamLib MoveCamToBackward"), Category = RTSUnitTemplate)
		void MoveCamToBackward();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MoveCamToLeft", Keywords = "TopDownRTSCamLib MoveCamToLeft"), Category = RTSUnitTemplate)
		void MoveCamToLeft();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MoveCamToRight", Keywords = "TopDownRTSCamLib MoveCamToRight"), Category = RTSUnitTemplate)
		void MoveCamToRight();
	
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "StartTime", Keywords = "TopDownRTSCamLib StartTime"), Category = RTSUnitTemplate)
		float StartTime = 0.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "CameraState", Keywords = "TopDownRTSTemplate CameraState"), Category = RTSUnitTemplate)
		TEnumAsByte<CameraData::CameraState> CameraState = CameraData::UseScreenEdges;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetCameraState", Keywords = "TopDownRTSCamLib SetCameraState"), Category = RTSUnitTemplate)
		void SetCameraState(TEnumAsByte<CameraData::CameraState> NewCameraState);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetCameraState", Keywords = "TopDownRTSCamLib GetCameraState"), Category = RTSUnitTemplate)
		TEnumAsByte<CameraData::CameraState> GetCameraState();


	
	// Control Widget
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (DisplayName = "ControlWidgetComp", Keywords = "RTSUnitTemplate ControlWidgetComp"), Category = RTSUnitTemplate)
		class UWidgetComponent* ControlWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ControlWidgetRotator", Keywords = "RTSUnitTemplate ControlWidgetRotator"), Category = RTSUnitTemplate)
		FRotator ControlWidgetRotation = FRotator(50, 180, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ControlWidgetLocation", Keywords = "RTSUnitTemplate ControlWidgetLocation"), Category = RTSUnitTemplate)
		FVector ControlWidgetLocation = FVector(400.f, -100.0f, -250.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ControlWidgetHideLocation", Keywords = "RTSUnitTemplate ControlWidgetHideLocation"), Category = RTSUnitTemplate)
		FVector ControlWidgetHideLocation = FVector(400.f, -2500.0f, -250.0f);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "HideControlWidget", Keywords = "RTSUnitTemplate HideControlWidget"), Category = RTSUnitTemplate)
		void HideControlWidget();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ShowControlWidget", Keywords = "RTSUnitTemplate ShowControlWidget"), Category = RTSUnitTemplate)
		void ShowControlWidget();



	/////// Loading Widget ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (DisplayName = "LoadingWidgetComp", Keywords = "RTSUnitTemplate LoadingWidgetComp"), Category = RTSUnitTemplate)
	class UWidgetComponent* LoadingWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "LoadingWidgetRotation", Keywords = "RTSUnitTemplate LoadingWidgetRotation"), Category = RTSUnitTemplate)
	FRotator LoadingWidgetRotation = FRotator(50.f, 180, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "LoadingWidgetLocation", Keywords = "RTSUnitTemplate LoadingWidgetLocation"), Category = RTSUnitTemplate)
	FVector LoadingWidgetLocation = FVector(150.f, 000.0f, -150.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "LoadingWidgetHideLocation", Keywords = "RTSUnitTemplate LoadingWidgetHideLocation"), Category = RTSUnitTemplate)
	FVector LoadingWidgetHideLocation = FVector(300.f, -2200.0f, -250.0f);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "HideLoadingWidget", Keywords = "RTSUnitTemplate HideLoadingWidget"), Category = RTSUnitTemplate)
		void DeSpawnLoadingWidget();
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SpawnLoadingWidget", Keywords = "RTSUnitTemplate SpawnLoadingWidget"), Category = RTSUnitTemplate)
		void SpawnLoadingWidget();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
