// Copyright 2022 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.
#include "Controller/CameraControllerBase.h"


ACameraControllerBase::ACameraControllerBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACameraControllerBase::BeginPlay()
{
	Super::BeginPlay();
	HUDBase = Cast<APathProviderHUD>(GetHUD());
	CameraBase = Cast<ACameraBase>(GetPawn());
	GetViewPortScreenSizes(1);
}

void ACameraControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ACameraControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckSpeakingUnits();
	CameraBaseMachine(DeltaSeconds);
}

bool ACameraControllerBase::CheckSpeakingUnits()
{


	for (int32 i = 0; i < HUDBase->SpeakingUnits.Num(); i++)
	{
		if(HUDBase->SpeakingUnits[i]->LockCamOnUnit)
		{
			SpeakingUnit = HUDBase->SpeakingUnits[i];
			SetCameraState(CameraData::LockOnSpeaking);
			return true;
		}
	}
	SpeakingUnit = nullptr;
	return false;
}

void ACameraControllerBase::SetCameraState(TEnumAsByte<CameraData::CameraState> NewCameraState)
{
	CameraBase->SetCameraState(NewCameraState);
}

void ACameraControllerBase::GetViewPortScreenSizes(int x)
{
	switch (x)
	{
	case 1:
		{
			GetViewportSize(CameraBase->ScreenSizeX, CameraBase->ScreenSizeY);
		}
		break;
	case 2:
		{
			CameraBase->ScreenSizeX = GSystemResolution.ResX;
			CameraBase->ScreenSizeY = GSystemResolution.ResY;
		}
		break;
	}
}


FVector ACameraControllerBase::GetCameraPanDirection() {
	float MousePosX = 0;
	float MousePosY = 0;
	float CamDirectionX = 0;
	float CamDirectionY = 0;

	GetMousePosition(MousePosX, MousePosY);

	const float CosYaw = FMath::Cos(CameraBase->YawValue*PI/180);
	const float SinYaw = FMath::Sin(CameraBase->YawValue*PI/180);
	
	if (MousePosX <= CameraBase->Margin)
	{
		CamDirectionY = -CosYaw;
		CamDirectionX = SinYaw;
	}
	if (MousePosY <= CameraBase->Margin)
	{
		CamDirectionX = CosYaw;
		CamDirectionY = SinYaw;
	}
	if (MousePosX >= CameraBase->ScreenSizeX - CameraBase->Margin)
	{
		CamDirectionY = CosYaw;
		CamDirectionX = -SinYaw;
	}
	if (MousePosY >= CameraBase->ScreenSizeY - CameraBase->Margin)
	{
		CamDirectionX = -CosYaw;
		CamDirectionY = -SinYaw;
	}
	
	return FVector(CamDirectionX, CamDirectionY, 0);
}


void ACameraControllerBase::CameraBaseMachine(float DeltaTime)
{
	//HUDBase->MoveActorsThroughWayPoints(MovingActors);
	if(CameraBase->BlockControls) return;
	
	FVector PanDirection = GetCameraPanDirection();
	
	if(CameraBase)
	{
		switch (CameraBase->GetCameraState())
		{
		case CameraData::UseScreenEdges:
			{
				//UE_LOG(LogTemp, Warning, TEXT("UseScreenEdges"));
				CameraBase->PanMoveCamera(PanDirection*CameraBase->CamSpeed);
				if(IsStrgPressed && AIsPressed) CameraBase->SetCameraState(CameraData::MoveLeft);
				else if(IsStrgPressed && DIsPressed) CameraBase->SetCameraState(CameraData::MoveRight);
				else if(IsStrgPressed && WIsPressed) CameraBase->SetCameraState(CameraData::MoveForward);
				else if(IsStrgPressed && SIsPressed) CameraBase->SetCameraState(CameraData::MoveBackward);
				else if(LockCameraToCharacter) CameraBase->SetCameraState(CameraData::LockOnCharacter);
			}
			break;
		case CameraData::MoveForward:
			{
				//UE_LOG(LogTemp, Warning, TEXT("MoveForward"));
				CameraBase->MoveCamToForward();
				if(IsStrgPressed && AIsPressed) CameraBase->MoveCamToLeft();
				else if(IsStrgPressed && DIsPressed) CameraBase->MoveCamToRight();
				
				if(!WIsPressed) CameraBase->SetCameraState(CameraData::UseScreenEdges);

			}
			break;
		case CameraData::MoveBackward:
			{
				//UE_LOG(LogTemp, Warning, TEXT("MoveBackward"));
				CameraBase->MoveCamToBackward();
				if(IsStrgPressed && AIsPressed) CameraBase->MoveCamToLeft();
				else if(IsStrgPressed && DIsPressed) CameraBase->MoveCamToRight();
				
				if(!SIsPressed) CameraBase->SetCameraState(CameraData::UseScreenEdges);

			}
			break;
		case CameraData::MoveLeft:
			{
				//UE_LOG(LogTemp, Warning, TEXT("MoveLeft"));
				CameraBase->MoveCamToLeft();
				if(IsStrgPressed && WIsPressed) CameraBase->MoveCamToForward();
				else if(IsStrgPressed && SIsPressed) CameraBase->MoveCamToBackward();
				
				if(!AIsPressed) CameraBase->SetCameraState(CameraData::UseScreenEdges);

			}
			break;
		case CameraData::MoveRight:
			{
				//UE_LOG(LogTemp, Warning, TEXT("MoveRight"));
				CameraBase->MoveCamToRight();
				if(WIsPressed) CameraBase->MoveCamToForward();
				else if(SIsPressed) CameraBase->MoveCamToBackward();

				if(!DIsPressed) CameraBase->SetCameraState(CameraData::UseScreenEdges);

			}
			break;
		case CameraData::ZoomIn:
			{
				//UE_LOG(LogTemp, Warning, TEXT("ZoomIn"));
				CameraBase->ZoomIn();

				if(LockCameraToCharacter) CameraBase->SetCameraState(CameraData::LockOnCharacter);
			}
			break;
		case CameraData::ZoomOut:
			{
				//UE_LOG(LogTemp, Warning, TEXT("ZoomOut"));
				CameraBase->ZoomOut();

				if(LockCameraToCharacter) CameraBase->SetCameraState(CameraData::LockOnCharacter);
			}
			break;
		case CameraData::ZoomOutPosition:
			{
				ZoomOutToPosition = true;
				//UE_LOG(LogTemp, Warning, TEXT("ZoomToPosition"));
				
				CameraBase->ZoomOutToPosition(CameraBase->ZoomOutPosition);

			}
			break;
		case CameraData::ZoomInPosition:
			{
				ZoomOutToPosition = false;
				ZoomInToPosition = true;
				//UE_LOG(LogTemp, Warning, TEXT("ZoomInPosition"));
				
				if(LockCameraToCharacter) CameraBase->SetCameraState(CameraData::LockOnCharacter);
				
				if(CameraBase->ZoomInToPosition(CameraBase->ZoomPosition))
				{
					ZoomInToPosition = false;
					if(LockCameraToCharacter) CameraBase->SetCameraState(CameraData::LockOnCharacter);
					else CameraBase->SetCameraState(CameraData::UseScreenEdges);
				}
			}
			break;
		case CameraData::RotateLeft:
			{
				CamIsRotatingRight = false;
				CamIsRotatingLeft = true;
				//UE_LOG(LogTemp, Warning, TEXT("RotateLeft"));

				if(LockCameraToCharacter) CameraBase->SetCameraState(CameraData::LockOnCharacter);
				
				if(CameraBase->RotateCamLeft(!LockCameraToCharacter, CameraBase->AddCamRotation))
				{
					CamIsRotatingLeft = false;
					if(!LockCameraToCharacter)CameraBase->SetCameraState(CameraData::UseScreenEdges);
				}
				
				SetControlRotation(FRotator(0.f, CameraBase->YawValue, 0.f));
			}
			break;
		case CameraData::RotateRight:
			{
				CamIsRotatingLeft = false;
				CamIsRotatingRight = true;
				//UE_LOG(LogTemp, Warning, TEXT("RotateRight"));

				if(LockCameraToCharacter) CameraBase->SetCameraState(CameraData::LockOnCharacter);
				if(CameraBase->RotateCamRight(!LockCameraToCharacter, CameraBase->AddCamRotation))
				{
					CamIsRotatingRight = false;
					if(!LockCameraToCharacter)CameraBase->SetCameraState(CameraData::UseScreenEdges);
				}

				SetControlRotation(FRotator(0.f, CameraBase->YawValue, 0.f));
				
			}
			break;
		case CameraData::LockOnCharacter:
			{
				//UE_LOG(LogTemp, Warning, TEXT("LockOnCharacter"));
				LockCamToCharacter(0);
				
			}
			break;
		case CameraData::LockOnSpeaking:
			{
				//UE_LOG(LogTemp, Warning, TEXT("LockOnSpeaking"));
				if(SpeakingUnit)
					LockCamToSpecificUnit(SpeakingUnit);
				else
					SetCameraState(CameraData::ZoomToNormalPosition);
			}
			break;
		case CameraData::ZoomToNormalPosition:
			{
				//UE_LOG(LogTemp, Warning, TEXT("ZoomToNormalPosition"));
				
				if(CameraBase->ZoomOutToPosition(CameraBase->ZoomPosition))
				{
					if(CameraBase->RotateCamRight(!LockCameraToCharacter, CameraBase->AddCamRotation))
					{
						CamIsRotatingRight = false;
						CamIsRotatingLeft = false;
						if(!LockCameraToCharacter)CameraBase->SetCameraState(CameraData::UseScreenEdges);
						else CameraBase->SetCameraState(CameraData::LockOnCharacter);
					}
					SetControlRotation(FRotator(0.f, CameraBase->YawValue, 0.f));
				};
			}
			break;
		case CameraData::ZoomToThirdPerson:
			{
				//UE_LOG(LogTemp, Warning, TEXT("ZoomToThirdPerson"));
				
				if( SelectedUnits.Num())
				{
					FVector SelectedActorLocation = SelectedUnits[0]->GetActorLocation();
				
					CameraBase->LockOnUnit(SelectedUnits[0]);
					if (!CameraBase->IsCameraInAngle())
					{
						CameraBase->RotateCamRight(true, CameraBase->AddCamRotation);
						SetControlRotation(FRotator(0.f, CameraBase->YawValue, 0.f)); 
					}else if(CameraBase->ZoomInToThirdPerson(SelectedActorLocation))
					{
						LockCameraToCharacter = false;
						CameraBase->CameraDistanceToCharacter = (CameraBase->GetActorLocation().Z - SelectedUnits[0]->GetActorLocation().Z);
						CameraBase->SetCameraState(CameraData::ThirdPerson);
					}
		
				}
			}
			break;
		case CameraData::ThirdPerson:
			{
			
				if( SelectedUnits.Num())
				{
					float YawActor = SelectedUnits[0]->GetActorRotation().Yaw;
					float YawCamera = CameraBase->GetActorRotation().Yaw;
	
					CameraBase->LockOnUnit(SelectedUnits[0]);
					
					if(YawCamera-YawActor < -90)
						CameraBase->RotateCamRightTo(YawActor, CameraBase->AddCamRotation/3);
					else if(YawCamera-YawActor > 90)
						CameraBase->RotateCamRightTo(YawActor, CameraBase->AddCamRotation/3);
					else if(YawCamera-YawActor < -25)
						CameraBase->RotateCamLeftTo(YawActor, CameraBase->AddCamRotation/3);
					else if(YawCamera-YawActor > 25)
						CameraBase->RotateCamRightTo(YawActor, CameraBase->AddCamRotation/3);

					SetControlRotation(FRotator(0.f, CameraBase->YawValue, 0.f));
					LockZDistanceToCharacter();
					
				}
			}
			break;
		default:
			{
				//UE_LOG(LogTemp, Warning, TEXT("default"));
				CameraBase->SetCameraState(CameraData::UseScreenEdges);
			}
			break;
		}
	}
}

void ACameraControllerBase::ToggleLockCamToCharacter()
{
	if(IsStrgPressed)
	{
		LockCameraToCharacter = !LockCameraToCharacter;
	
		if(LockCameraToCharacter)
			CameraBase->SetCameraState(CameraData::LockOnCharacter);
		else
		{
			CameraBase->SetCameraState(CameraData::UseScreenEdges);
		}
	}
}
void ACameraControllerBase::UnlockCamFromCharacter()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit);

	if (Hit.bBlockingHit)
	{
		AUnitBase* CharacterFound = Cast<AUnitBase>(Hit.GetActor());

		if(!CharacterFound){
			CameraBase->SetCameraState(CameraData::UseScreenEdges);
			LockCameraToCharacter = false;
		}
	}
	
}

void ACameraControllerBase::LockCamToSpecificUnit(AUnitBase* SUnit)
{
	ASpeakingUnit* Unit = Cast<ASpeakingUnit>(SUnit);
	
	if( Unit)
	{
		FVector SelectedActorLocation = Unit->GetActorLocation();
		
		CameraBase->LockOnUnit(Unit);

		CamIsRotatingLeft = true;
		CameraBase->RotateCamLeft(false, CameraBase->AddCamRotation/100);
		SetControlRotation(FRotator(0.f, CameraBase->YawValue, 0.f));
		
		if(CamIsZoomingIn)
		{
			CameraBase->ZoomIn();
		}else if(CamIsZoomingOut)
			CameraBase->ZoomOut();
		else if(ZoomOutToPosition) CameraBase->ZoomOutToPosition(CameraBase->ZoomOutPosition, SelectedActorLocation);
		else if(ZoomInToPosition && CameraBase->ZoomInToPosition(CameraBase->ZoomPosition,SelectedActorLocation)) ZoomInToPosition = false;
		else if(CameraBase->IsCharacterDistanceTooHigh(Unit->SpeakZoomPosition, SelectedActorLocation))
		{
			CameraBase->ZoomInToPosition(Unit->SpeakZoomPosition, SelectedActorLocation);
			CameraBase->CameraDistanceToCharacter = (CameraBase->GetActorLocation().Z - Unit->GetActorLocation().Z);
		}

	}else
	{ 
		LockCameraToCharacter = !LockCameraToCharacter;
		CameraBase->SetCameraState(CameraData::ZoomInPosition);
	}
}

void ACameraControllerBase::LockCamToCharacter(int Index)
{
	if( SelectedUnits.Num())
	{
		FVector SelectedActorLocation = SelectedUnits[Index]->GetActorLocation();
		
		CameraBase->LockOnUnit(SelectedUnits[Index]);
		if(!CamIsRotatingLeft && (CamIsRotatingRight || !CameraBase->IsCameraInAngle()))
		{
			CamIsRotatingLeft = false;
			if(CameraBase->RotateCamRight(!LockCameraToCharacter, CameraBase->AddCamRotation)) CamIsRotatingRight = false;
			SetControlRotation(FRotator(0.f, CameraBase->YawValue, 0.f)); 
		}
		if(CamIsRotatingLeft)
		{
			CamIsRotatingRight = false;
			if(CameraBase->RotateCamLeft(!LockCameraToCharacter, CameraBase->AddCamRotation)) CamIsRotatingLeft = false;
			SetControlRotation(FRotator(0.f, CameraBase->YawValue, 0.f));
		}
		
		if(CamIsZoomingIn)
		{
			CameraBase->ZoomIn();
		}else if(CamIsZoomingOut)
			CameraBase->ZoomOut();
		else if(ZoomOutToPosition) CameraBase->ZoomOutToPosition(CameraBase->ZoomOutPosition, SelectedActorLocation);
		else if(ZoomInToPosition && CameraBase->ZoomInToPosition(CameraBase->ZoomPosition,SelectedActorLocation)) ZoomInToPosition = false;
		else if(CameraBase->IsCharacterDistanceTooLow(CameraBase->ZoomPosition, SelectedActorLocation) && !CamIsZoomingIn && !HoldZoomOnLockedCharacter)
		{
				CameraBase->ZoomOutToPosition(CameraBase->ZoomPosition, SelectedActorLocation);
				CameraBase->CameraDistanceToCharacter = (CameraBase->GetActorLocation().Z - SelectedUnits[0]->GetActorLocation().Z);
		}else if(!CamIsZoomingIn && !CamIsZoomingOut && !HoldZoomOnLockedCharacter)
		{
			LockZDistanceToCharacter();
		}
	}else
	{ 
		LockCameraToCharacter = !LockCameraToCharacter;
		CameraBase->SetCameraState(CameraData::ZoomInPosition);
	}
}




void ACameraControllerBase::LockZDistanceToCharacter()
{
	if(ZoomInToPosition == false &&
		ZoomOutToPosition == false &&
		CamIsZoomingIn == false &&
		CamIsZoomingOut == false &&
		CameraBase &&
		SelectedUnits.Num())
	{
		
		const FVector SelectedActorLocation = SelectedUnits[0]->GetActorLocation();
		const FVector CameraBaseLocation = CameraBase->GetActorLocation();
		
		const float NewCameraDistanceToCharacter = (CameraBaseLocation.Z - SelectedActorLocation.Z);
		float ZChange = CameraBase->CameraDistanceToCharacter - NewCameraDistanceToCharacter;
		
		const float CosYaw = FMath::Cos(CameraBase->YawValue*PI/180);
		const float SinYaw = FMath::Sin(CameraBase->YawValue*PI/180);
		const FVector NewPawnLocation = FVector(SelectedActorLocation.X - CameraBase->CameraDistanceToCharacter * 0.7*CosYaw, SelectedActorLocation.Y - CameraBase->CameraDistanceToCharacter * 0.7*SinYaw, CameraBaseLocation.Z+ZChange);

		CameraBase->SetActorLocation(NewPawnLocation);
		CameraBase->CameraDistanceToCharacter = (CameraBase->GetActorLocation().Z - SelectedUnits[0]->GetActorLocation().Z);
	}
}
