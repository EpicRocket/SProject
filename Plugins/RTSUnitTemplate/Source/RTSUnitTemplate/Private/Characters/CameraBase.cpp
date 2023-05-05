// Copyright 2022 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.


#include "Characters/CameraBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Controller/Input/EnhancedInputComponentBase.h"
#include "Controller/Input/GameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "Controller/CameraControllerBase.h"
#include "UnrealEngine.h"
#include "Kismet/KismetMathLibrary.h"

ACameraBase::ACameraBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CreateCameraComp();

	if (RootComponent == nullptr) {
		RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	}

	ControlWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("ControlWidget"));
	ControlWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	LoadingWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("LoadingWidget"));
	LoadingWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	SetActorEnableCollision(false);
	
}

// Called when the game starts or when spawned
void ACameraBase::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();

			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(MappingContext, MappingPriority);
		}
		
	}
	
	SpawnControllWidget();
	SpawnLoadingWidget();
}

// Called every frame
void ACameraBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACameraBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponentBase* EnhancedInputComponentBase = Cast<UEnhancedInputComponentBase>(PlayerInputComponent);

	if(EnhancedInputComponentBase)
	{
		check(EnhancedInputComponentBase);
		const FGameplayTags& GameplayTags = FGameplayTags::Get();

		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Tab_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::Input_Tab_Pressed, 0);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Tab_Released, ETriggerEvent::Triggered, this, &ACameraBase::Input_Tab_Released, 0);
		
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_LeftClick_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::Input_LeftClick_Pressed, 0);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_LeftClick_Released, ETriggerEvent::Triggered, this, &ACameraBase::Input_LeftClick_Released, 0);

		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_RightClick_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::Input_RightClick_Pressed, 0);

		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_G_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::Input_G_Pressed, 0);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_A_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::Input_A_Pressed, 0);
		
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Shift_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::Input_Shift_Pressed, 0);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Shift_Released, ETriggerEvent::Triggered, this, &ACameraBase::Input_Shift_Released, 0);
		
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Ctrl_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::Input_Ctrl_Pressed, 0);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Ctrl_Released, ETriggerEvent::Triggered, this, &ACameraBase::Input_Ctrl_Released, 0);
		
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Space_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 7);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Space_Released, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 8);

		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_W_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 1);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_W_Released, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 0);
		
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_S_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 2);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_S_Released, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 0);

		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_A_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 3);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_A_Released, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 0);

		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_D_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 4);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_D_Released, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 0);

		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_X_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 5);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_X_Released, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 0);

		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Y_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 6);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Y_Released, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 0);

		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_Q_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 9);
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_E_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 10);
		
		EnhancedInputComponentBase->BindActionByTag(InputConfig, GameplayTags.InputTag_T_Pressed, ETriggerEvent::Triggered, this, &ACameraBase::SwitchControllerStateMachine, 12);
		
	}

	
}

void ACameraBase::CreateCameraComp()
{
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootScene);
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetRelativeRotation(SpringArmRotator);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArm);
}

void ACameraBase::SpawnControllWidget()
{
	FTransform SpellTransform;
	SpellTransform.SetLocation(FVector(500, 0, 0));
	SpellTransform.SetRotation(FQuat(FRotator::ZeroRotator));


	if (ControlWidgetComp) {
		FRotator NewRotation = ControlWidgetRotation;
		FQuat QuatRotation = FQuat(NewRotation);
		ControlWidgetComp->SetRelativeRotation(QuatRotation, false, 0, ETeleportType::None);
		ControlWidgetComp->SetRelativeLocation(ControlWidgetLocation);
	}
}


void ACameraBase::PanMoveCamera(const FVector& NewPanDirection) {
	if (NewPanDirection != FVector::ZeroVector) {
		AddActorWorldOffset(NewPanDirection * GetActorLocation().Z * 0.001);
	}
}

void ACameraBase::ZoomOut() {
	AddActorWorldOffset(FVector(0, 0, 0.3) * CamSpeed);
}

void ACameraBase::ZoomIn() {
	if(GetActorLocation().Z > 0.f)
		AddActorWorldOffset(FVector(0, 0, -0.3) * CamSpeed);
}



bool ACameraBase::RotateCamLeft(bool UseCamRotationOffset, float Add) // CamRotationOffset
{
	
	
	YawValue += Add;

	if(Add >= 1.0f)
		YawValue = floor(YawValue+0.5);
	
	if(UseCamRotationOffset)
	{
		if (YawValue <= CameraAngles[0] + RotationDegreeStep) {
			AddActorWorldOffset(FVector(+CamRotationOffset, -CamRotationOffset, 0));
		}
		else if (YawValue <= CameraAngles[1] + RotationDegreeStep) {
			AddActorWorldOffset(FVector(+CamRotationOffset, +CamRotationOffset, 0));
		}
		else if (YawValue <= CameraAngles[2] + RotationDegreeStep) {
			AddActorWorldOffset(FVector(-CamRotationOffset, +CamRotationOffset, 0));
		}
		else if (YawValue <= CameraAngles[3] + RotationDegreeStep) {
			AddActorWorldOffset(FVector(-CamRotationOffset, -CamRotationOffset, 0));
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("RotateCamLeft %f"), YawValue);
	
	if (YawValue == 360) YawValue = 0.f;
	if (YawValue == CameraAngles[0] || YawValue == CameraAngles[3] || YawValue == CameraAngles[2] || YawValue == CameraAngles[1]) return true;

		
	
	return false;
}

bool ACameraBase::RotateCamRight(bool UseCamRotationOffset, float Add) // CamRotationOffset
{
	
	YawValue -= Add;

	if(Add >= 1.0f)
		YawValue = floor(YawValue+0.5);
	
	if(UseCamRotationOffset)
	{
		if (YawValue >= CameraAngles[3]) {
			AddActorWorldOffset(FVector(+CamRotationOffset, +CamRotationOffset, 0));
		}
		else if (YawValue >= CameraAngles[2]) {
			AddActorWorldOffset(FVector(+CamRotationOffset, -CamRotationOffset, 0));
		}
		else if (YawValue >= CameraAngles[1]) {
			AddActorWorldOffset(FVector(-CamRotationOffset, -CamRotationOffset, 0));
		}
		else if (YawValue >= CameraAngles[0]) {
			AddActorWorldOffset(FVector(-CamRotationOffset, +CamRotationOffset, 0));
		}
	}
	if (YawValue == -1) YawValue = 359.f;
	if (YawValue == CameraAngles[0] || YawValue == CameraAngles[3] || YawValue == CameraAngles[2] || YawValue == CameraAngles[1]) return true;
	
	return false;
}

bool ACameraBase::RotateCamLeftTo(float Position, float Add)
{
	if (abs(YawValue - Position) <= 1.f) return true;
	
	YawValue += Add;
	if (YawValue == 360) YawValue = 0.f;
	if (YawValue > CameraAngles[3]+RotationDegreeStep) YawValue -= CameraAngles[3]+RotationDegreeStep;
	
	return false;
}

bool ACameraBase::RotateCamRightTo(float Position, float Add)
{
	
	if (abs(YawValue - Position) <= 1.f) return true;
	
	YawValue -= Add;
	if (YawValue == -1) YawValue = 359.f;
	if (YawValue < CameraAngles[0]) YawValue += CameraAngles[3]+RotationDegreeStep;
	
	return false;
}

FVector2D ACameraBase::GetMousePos2D()
{
	float PosX;
	float PosY;
	PC->GetMousePosition(PosX, PosY);

	return FVector2D(PosX, PosY);
}


void ACameraBase::Zoom()
{
	FVector2D MPosition = GetMousePos2D();
	FVector ActorLocation = GetActorLocation();
	SetActorLocation(FVector(MPosition.X, MPosition.Y, ActorLocation.Z));
}

bool ACameraBase::ZoomOutToPosition(float Distance, const FVector SelectedActorPosition)
{
	FVector ALocation = GetActorLocation();

	if (ALocation.Z - SelectedActorPosition.Z < Distance)
	{
		const float CosYaw = FMath::Cos(YawValue*PI/180);
		const float SinYaw = FMath::Sin(YawValue*PI/180);
	
		const FVector NewPawnLocation = FVector(4.25*CosYaw*(-1),  4.25*SinYaw*(-1), 6);

		AddActorWorldOffset(NewPawnLocation * CamSpeed);
		return false;
	}

	return true;
}

bool ACameraBase::ZoomInToPosition(float Distance, const FVector SelectedActorPosition)
{
	const FVector ALocation = GetActorLocation();
	if (ALocation.Z - SelectedActorPosition.Z > Distance)
	{
		const float CosYaw = FMath::Cos(YawValue*PI/180);
		const float SinYaw = FMath::Sin(YawValue*PI/180);
	
		const FVector NewPawnLocation = FVector(4.25*CosYaw,  4.25*SinYaw, -6);

		AddActorWorldOffset(NewPawnLocation * CamSpeed);
		return false;
	}
	return true;
}

void ACameraBase::LockOnUnit(AUnitBase* Unit)
{
	if (Unit) {

		const FVector SelectedActorLocation = Unit->GetActorLocation();
		const FVector CurrentCameraPawnLocation = GetActorLocation();
		const float ZDistanceToSelectedActor = CurrentCameraPawnLocation.Z - SelectedActorLocation.Z;
		
		const float CosYaw = FMath::Cos(YawValue*PI/180);
		const float SinYaw = FMath::Sin(YawValue*PI/180);
		const FVector NewPawnLocation = FVector(SelectedActorLocation.X - ZDistanceToSelectedActor * 0.7*CosYaw, SelectedActorLocation.Y - ZDistanceToSelectedActor * 0.7*SinYaw, CurrentCameraPawnLocation.Z);

		SetActorLocation(NewPawnLocation);

	}
}

bool ACameraBase::IsCharacterDistanceTooLow(float Distance, const FVector SelectedActorPosition)
{
		
	FVector ALocation = GetActorLocation();
	if(ALocation.Z - SelectedActorPosition.Z < Distance)
	{
		return true;
	}
	return false;
}

bool ACameraBase::IsCharacterDistanceTooHigh(float Distance, const FVector SelectedActorPosition)
{
		
	FVector ALocation = GetActorLocation();
	if(ALocation.Z - SelectedActorPosition.Z > Distance)
	{
		return true;
	}
	return false;
}

bool ACameraBase::ZoomInToThirdPerson(const FVector SelectedActorPosition)
{
	FVector ALocation = GetActorLocation();
	if (ALocation.Z - SelectedActorPosition.Z > ZoomThirdPersonPosition) {
		const float CosYaw = FMath::Cos(YawValue*PI/180);
		const float SinYaw = FMath::Sin(YawValue*PI/180);
	
		const FVector NewPawnLocation = FVector(4.25*CosYaw,  4.25*SinYaw, -6);

		AddActorWorldOffset(NewPawnLocation * CamSpeed);

		return false;
	}
	return true;
}

void ACameraBase::HideControlWidget()
{
	if (ControlWidgetComp)
		ControlWidgetComp->SetRelativeLocation(ControlWidgetHideLocation);
}

void ACameraBase::ShowControlWidget()
{
	if (ControlWidgetComp)
		ControlWidgetComp->SetRelativeLocation(ControlWidgetLocation);
}

void ACameraBase::DeSpawnLoadingWidget()
{
	if (LoadingWidgetComp)
	{
		LoadingWidgetComp->DestroyComponent();
	}
}

void ACameraBase::SpawnLoadingWidget()
{
	FTransform SpellTransform;
	SpellTransform.SetLocation(FVector(500, 0, 0));
	SpellTransform.SetRotation(FQuat(FRotator::ZeroRotator));


	if (LoadingWidgetComp) {
		FRotator NewRotation = LoadingWidgetRotation;
		FQuat QuatRotation = FQuat(NewRotation);
		LoadingWidgetComp->SetRelativeRotation(QuatRotation, false, 0, ETeleportType::None);
		LoadingWidgetComp->SetRelativeLocation(LoadingWidgetLocation);
	}
}


void ACameraBase::SetCameraState(TEnumAsByte<CameraData::CameraState> NewCameraState)
{
	CameraState = NewCameraState;
}

TEnumAsByte<CameraData::CameraState> ACameraBase::GetCameraState()
{
	return CameraState;
}

void ACameraBase::Input_LeftClick_Pressed(const FInputActionValue& InputActionValue, int32 Camstate)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	if(CameraControllerBase)
	{
		CameraControllerBase->LeftClickPressed();
		CameraControllerBase->JumpCamera();
	}
}

void ACameraBase::Input_LeftClick_Released(const FInputActionValue& InputActionValue, int32 Camstate)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	if(CameraControllerBase)
	{
		CameraControllerBase->LeftClickReleased();
	}
}

void ACameraBase::Input_RightClick_Pressed(const FInputActionValue& InputActionValue, int32 Camstate)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	if(CameraControllerBase)
	{
		CameraControllerBase->RightClickPressed();
	}
}

void ACameraBase::Input_G_Pressed(const FInputActionValue& InputActionValue, int32 Camstate)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	if(CameraControllerBase)
	{
		CameraControllerBase->ToggleLockCamToCharacter();
	}
}

void ACameraBase::Input_A_Pressed(const FInputActionValue& InputActionValue, int32 CamState)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	if(CameraControllerBase)
	{
		CameraControllerBase->APressed();
	}
}

void ACameraBase::Input_Ctrl_Pressed(const FInputActionValue& InputActionValue, int32 Camstate)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	if(CameraControllerBase)
	{
		CameraControllerBase->IsStrgPressed = true;
	}
}

void ACameraBase::Input_Ctrl_Released(const FInputActionValue& InputActionValue, int32 Camstate)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	if(CameraControllerBase)
	{
		CameraControllerBase->IsStrgPressed = false;
	}
}

void ACameraBase::Input_Tab_Pressed(const FInputActionValue& InputActionValue, int32 CamState)
{
	if(BlockControls) return;
	
	ShowControlWidget();
}

void ACameraBase::Input_Tab_Released(const FInputActionValue& InputActionValue, int32 CamState)
{
	if(BlockControls) return;
	
	HideControlWidget();
}

void ACameraBase::Input_Shift_Pressed(const FInputActionValue& InputActionValue, int32 CamState)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	
	if(CameraControllerBase)
	{
		CameraControllerBase->ShiftPressed();
	}
}

void ACameraBase::Input_Shift_Released(const FInputActionValue& InputActionValue, int32 CamState)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	
	if(CameraControllerBase)
	{
		CameraControllerBase->ShiftReleased();
	}
}

void ACameraBase::JumpCamera(FHitResult Hit)
{
	FVector ActorLocation = GetActorLocation();

	const float CosYaw = FMath::Cos(YawValue*PI/180);
	const float SinYaw = FMath::Sin(YawValue*PI/180);
	const FVector NewPawnLocation = FVector(Hit.Location.X - ActorLocation.Z * 0.7*CosYaw,  Hit.Location.Y - ActorLocation.Z * 0.7*SinYaw, ActorLocation.Z);

	SetActorLocation(NewPawnLocation);
}


void ACameraBase::MoveCamToForward()
{
	const float CosYaw = FMath::Cos(YawValue*PI/180);
	const float SinYaw = FMath::Sin(YawValue*PI/180);
	
	const FVector NewPawnLocation = FVector(0.3*CosYaw,  0.3*SinYaw, 0);
	
	AddActorWorldOffset(NewPawnLocation * CamSpeed);
}

void ACameraBase::MoveCamToBackward()
{
	const float CosYaw = FMath::Cos(YawValue*PI/180);
	const float SinYaw = FMath::Sin(YawValue*PI/180);
	
	const FVector NewPawnLocation = FVector(0.3*CosYaw*(-1),  0.3*SinYaw*(-1), 0);
	
	AddActorWorldOffset(NewPawnLocation * CamSpeed);
}

void ACameraBase::MoveCamToLeft()
{

	const float CosYaw = FMath::Cos(YawValue*PI/180);
	const float SinYaw = FMath::Sin(YawValue*PI/180);
	
	const FVector NewPawnLocation = FVector(0.3*SinYaw,  0.3*CosYaw*(-1), 0);
	
	AddActorWorldOffset(NewPawnLocation * CamSpeed);
}

void ACameraBase::MoveCamToRight()
{
	const float CosYaw = FMath::Cos(YawValue*PI/180);
	const float SinYaw = FMath::Sin(YawValue*PI/180);
	
	const FVector NewPawnLocation = FVector(0.3*SinYaw*(-1),  0.3*CosYaw, 0);
	
	AddActorWorldOffset(NewPawnLocation * CamSpeed);
}

void ACameraBase::SwitchControllerStateMachine(const FInputActionValue& InputActionValue, int32 NewCameraState)
{
	if(BlockControls) return;
	
	ACameraControllerBase* CameraControllerBase = Cast<ACameraControllerBase>(GetController());
	
	if(CameraControllerBase)
	{
		if(CameraControllerBase->IsStrgPressed)
			switch (NewCameraState)
			{
		case 0:
			{
				CameraControllerBase->WIsPressed = false;
				CameraControllerBase->SIsPressed = false;
				CameraControllerBase->AIsPressed = false;
				CameraControllerBase->DIsPressed = false;
				
				if(CameraControllerBase->LockCameraToCharacter)
				{
					CameraControllerBase->CamIsZoomingIn = false;
					CameraControllerBase->CamIsZoomingOut = false;
				}
				else
					SetCameraState(CameraData::UseScreenEdges);
			}
				break;
		case 1:
			{
				CameraControllerBase->WIsPressed = true;
				SetCameraState(CameraData::MoveForward);
			} break;
		case 2:
			{
				CameraControllerBase->SIsPressed = true;
				SetCameraState(CameraData::MoveBackward);
			} break;
		case 3:
			{
				CameraControllerBase->AIsPressed = true;
				SetCameraState(CameraData::MoveLeft);
			} break;
		case 4:
			{
				CameraControllerBase->DIsPressed = true;
				SetCameraState(CameraData::MoveRight);
			} break;
		case 5:
			{
				if(CameraControllerBase->LockCameraToCharacter)
				{
					if(!CameraControllerBase->HoldZoomOnLockedCharacter) CameraControllerBase->CamIsZoomingIn = true;
				
					CameraControllerBase->HoldZoomOnLockedCharacter = !CameraControllerBase->HoldZoomOnLockedCharacter;
				
				} else
				{
					CameraControllerBase->CamIsZoomingIn = true;
					SetCameraState(CameraData::ZoomIn);
				}
			}
				break;
		case 6:
			{
				if(CameraControllerBase->LockCameraToCharacter)
				{
					if(!CameraControllerBase->HoldZoomOnLockedCharacter) CameraControllerBase->CamIsZoomingOut = true;
				
					CameraControllerBase->HoldZoomOnLockedCharacter = !CameraControllerBase->HoldZoomOnLockedCharacter;
				
				} else
				{
					CameraControllerBase->CamIsZoomingOut = true;
					SetCameraState(CameraData::ZoomOut);
				}
			

			}
				break;
		case 7: SetCameraState(CameraData::ZoomOutPosition); break;
		case 9: SetCameraState(CameraData::RotateLeft); break;
		case 10: SetCameraState(CameraData::RotateRight); break;
		case 11: SetCameraState(CameraData::LockOnCharacter); break;
		case 12: SetCameraState(CameraData::ZoomToThirdPerson); break;
		default: SetCameraState(CameraData::UseScreenEdges); break;
		}

		switch (NewCameraState)
		{
		case 8: SetCameraState(CameraData::ZoomInPosition); break;
		}
	}

}
