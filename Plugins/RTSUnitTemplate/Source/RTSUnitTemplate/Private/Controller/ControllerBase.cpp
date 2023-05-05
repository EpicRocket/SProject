// Copyright 2022 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.


#include "Controller/ControllerBase.h"
#include "GameFramework/CharacterMovementComponent.h"


AControllerBase::AControllerBase() {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}


void AControllerBase::BeginPlay() {
	
		CameraBase = Cast<ACameraBase>(GetPawn());
		HUDBase = Cast<APathProviderHUD>(GetHUD());
}


void AControllerBase::SetupInputComponent() {

	Super::SetupInputComponent();

}


void AControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HUDBase)
	if(HUDBase->CreatedGridAndDijkstra || HUDBase->StopLoading)
	{
		CameraBase->BlockControls = false;
		CameraBase->DeSpawnLoadingWidget();
	}

	if(LeftClickIsPressed)
	{
		FHitResult Hit_CPoint;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit_CPoint);
		HUDBase->CPoint = Hit_CPoint.Location;
	}
	
	FHitResult Hit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);


	
	if(AttackToggled)
	for (int32 i = 0; i < SelectedUnits.Num(); i++)
	{
		if(SelectedUnits[i] && SelectedUnits[i]->GetUnitState() == UnitData::Idle && SelectedUnits[i]->ToggleUnitDetection)
		HUDBase->ControllDirectionToMouse(SelectedUnits[i], Hit);
	}

	TArray<FPathPoint> PathPoints;
	if(!HUDBase->DisablePathFindingOnEnemy)
	for (int32 i = 0; i < HUDBase->EnemyUnitBases.Num(); i++)
		if( HUDBase->EnemyUnitBases[i] && !HUDBase->EnemyUnitBases[i]->IsFriendly &&  HUDBase->EnemyUnitBases[i]->DijkstraSetPath)
		{
			SetRunLocationUseDijkstraForAI(HUDBase->EnemyUnitBases[i]->DijkstraEndPoint, HUDBase->EnemyUnitBases[i]->DijkstraStartPoint, HUDBase->EnemyUnitBases, PathPoints, i);
			HUDBase->EnemyUnitBases[i]->DijkstraSetPath = false;
		}
	

	if (CameraBase && SelectedUnits.Num() && LockCameraToUnit)
		CameraBase->LockOnUnit(SelectedUnits[0]);
	
}

void AControllerBase::ShiftPressed()
{
	IsShiftPressed = true;
}

void AControllerBase::ShiftReleased()
{
	IsShiftPressed = false;
}

void AControllerBase::LeftClickPressed()
{
	LeftClickIsPressed = true;
	
	if (AttackToggled) {
		AttackToggled = false;
		for (int32 i = 0; i < SelectedUnits.Num(); i++)
		{
			if (SelectedUnits[i] && SelectedUnits[i]->UnitState != UnitData::Dead) {
				FHitResult Hit;
				GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
				
				FHitResult Hit_Pawn;
				GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit_Pawn);

				if (Hit_Pawn.bBlockingHit)
				{
					AUnitBase* UnitBase = Cast<AUnitBase>(Hit_Pawn.GetActor());
					
					if(UnitBase && !UnitBase->IsFriendly)
					{
						/// Focus Enemy Units ///
						SelectedUnits[i]->UnitToChase = UnitBase;
						SelectedUnits[i]->SetUnitState(UnitData::Chase);
					}else
					{
						/// A-Move Units ///
						FVector RunLocation = Hit.Location + FVector(i / 2 * 100, i % 2 * 100, 0);
						DrawDebugSphere(GetWorld(), RunLocation, 15, 5, FColor::Green, false, 1.5, 0, 1);
						
						SelectedUnits[i]->SetUnitState(UnitData::Run);
						SelectedUnits[i]->RunLocationArray.Empty();
						SelectedUnits[i]->RunLocationArrayIterator = 0;
						SelectedUnits[i]->RunLocation = RunLocation;
						SelectedUnits[i]->UnitStatePlaceholder = UnitData::Run;
					}
				}
				
			}
			
		}
		
	}
	else {

		FHitResult Hit_IPoint;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit_IPoint);
		HUDBase->IPoint = Hit_IPoint.Location;
		
		HUDBase->DeselectAllUnits();

		for (int32 i = 0; i < SelectedUnits.Num(); i++)
		{
			if(SelectedUnits[i])
			SelectedUnits[i]->SetDeselected();
		}
		
		SelectedUnits.Empty();
		FHitResult Hit_Pawn;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit_Pawn);
		
		if (Hit_Pawn.bBlockingHit)
		{
			AUnitBase* UnitBase = Cast<AUnitBase>(Hit_Pawn.GetActor());
			const ASpeakingUnit* SUnit = Cast<ASpeakingUnit>(Hit_Pawn.GetActor());
			
			if (UnitBase && UnitBase->IsFriendly && !SUnit)
				HUDBase->SetUnitSelected(UnitBase);
			else {
				HUDBase->InitialPoint = HUDBase->GetMousePos2D();
				HUDBase->bSelectFriendly = true;
			}
		}

	}

}

void AControllerBase::LeftClickReleased()
{
	LeftClickIsPressed = false;
	HUDBase->bSelectFriendly = false;
	SelectedUnits = HUDBase->SelectedUnits;
}


void AControllerBase::RightClickPressed()
{
	AttackToggled = false;
	if (IsShiftPressed) {
		for (int32 i = 0; i < SelectedUnits.Num(); i++) {
			if (SelectedUnits[i] && SelectedUnits[i]->UnitState != UnitData::Dead) {
				FHitResult Hit;
				GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
				FVector RunLocation = Hit.Location + FVector(i / 2 * 100, i % 2 * 100, 0);
				DrawDebugSphere(GetWorld(), RunLocation, 15, 5, FColor::Green, false, 1.5, 0, 1);
				if(!SelectedUnits[i]->RunLocationArray.Num())
				{
					SelectedUnits[i]->RunLocation = RunLocation;
					SelectedUnits[i]->SetUnitState(UnitData::Run);
				}
				
				SelectedUnits[i]->RunLocationArray.Add(RunLocation);
				SelectedUnits[i]->UnitStatePlaceholder = UnitData::Idle;
				SelectedUnits[i]->UnitsToChase.Empty();
				SelectedUnits[i]->UnitToChase = nullptr;
				SelectedUnits[i]->ToggleUnitDetection = false;
			}
			
		}
	}
	else {
		TArray<FPathPoint> PathPoints;
		for (int32 i = 0; i < SelectedUnits.Num(); i++) {
			if (SelectedUnits[i] && SelectedUnits[i]->UnitState != UnitData::Dead) {
				FVector UnitLocation = SelectedUnits[i]->GetActorLocation();
				
				FHitResult Hit;
				GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
				FVector RunLocation = Hit.Location + FVector(i / 2 * 100, i % 2 * 100, 0.f);

				DrawDebugCircle(GetWorld(), FVector(RunLocation.X, RunLocation.Y, RunLocation.Z+2.f), 15, 5, FColor::Green, false, 1.5, 0, 1, FVector(0, 1, 0), FVector(1, 0, 0));
				if(SelectedUnits[i]->GetUnitState() != UnitData::Run)
				SelectedUnits[i]->SetWalkSpeed(0.f);
				
				SelectedUnits[i]->SetUnitState(UnitData::Run);
				SelectedUnits[i]->RunLocationArray.Empty();
				SelectedUnits[i]->RunLocationArrayIterator = 0;
				SelectedUnits[i]->RunLocation = RunLocation;
				SelectedUnits[i]->UnitStatePlaceholder = UnitData::Idle;
				SelectedUnits[i]->ToggleUnitDetection = false;

				float Range = FVector::Dist(UnitLocation, Hit.Location);

				if(!HUDBase->DisablePathFindingOnFriendly && Range >= HUDBase->RangeThreshold && !HUDBase->IsLocationInNoPathFindingAreas(Hit.Location))
					SetRunLocationUseDijkstra(Hit.Location, UnitLocation, SelectedUnits, PathPoints, i);
			}
		}
	}
}


void AControllerBase::SetRunLocationUseDijkstra(FVector HitLocation, FVector UnitLocation, TArray <AUnitBase*> Units, TArray<FPathPoint>& PathPoints, int i)
{
	HUDBase->SetNextDijkstraTo(Units, HitLocation);
	FHitResult LineHit;
	FVector LineEndLocation = FVector(HitLocation.X, HitLocation.Y, HitLocation.Z+10.f);
	GetWorld()->LineTraceSingleByChannel(LineHit, UnitLocation, LineEndLocation, HUDBase->TraceChannelProperty, HUDBase->QueryParams);
	
	float ZDistance_B = abs(Units[i]->Next_DijkstraPMatrixes.CenterPoint.Z - HitLocation.Z);
		
	if(LineHit.bBlockingHit && Units[i]->Next_DijkstraPMatrixes.Matrix.Num())
	{
		if(i == 0 && HUDBase->UseDijkstraOnlyOnFirstUnit)
		{
			if(ZDistance_B > 150.f) HUDBase->SetDijkstraWithClosestZDistance(Units[i], HitLocation.Z);
			PathPoints = HUDBase->GetPathReUseDijkstra( Units[i]->Next_DijkstraPMatrixes.Matrix, HitLocation, UnitLocation);
		}

		if(!HUDBase->UseDijkstraOnlyOnFirstUnit)
		{
			if(ZDistance_B > 150.f) HUDBase->SetDijkstraWithClosestZDistance(Units[i], HitLocation.Z);
			PathPoints = HUDBase->GetPathReUseDijkstra( Units[i]->Next_DijkstraPMatrixes.Matrix, HitLocation, UnitLocation);
		}
					
		if(PathPoints.Num())
		{
			Units[i]->RunLocation = PathPoints[0].Point + FVector(i / 2 * 50, i % 2 * 50, 0.f);;
					
			for(int k = 1; k < PathPoints.Num(); k++)
			{
				Units[i]->RunLocationArray.Add(PathPoints[k].Point);
			}
						
			Units[i]->RunLocationArray.Add(HitLocation + FVector(i / 2 * 50, i % 2 * 50, 0.f));
		}
	}
}

void AControllerBase::SetRunLocationUseDijkstraForAI(FVector HitLocation, FVector UnitLocation, TArray <AUnitBase*> Units, TArray<FPathPoint>& PathPoints, int i)
{
	FHitResult LineHit;
	FVector LineEndLocation = FVector(HitLocation.X, HitLocation.Y, HitLocation.Z+10.f);
	GetWorld()->LineTraceSingleByChannel(LineHit, UnitLocation, LineEndLocation, HUDBase->TraceChannelProperty, HUDBase->QueryParams);
	
	float ZDistance_B = abs(Units[i]->Next_DijkstraPMatrixes.CenterPoint.Z - HitLocation.Z);
		
	if(LineHit.bBlockingHit && Units[i]->Next_DijkstraPMatrixes.Matrix.Num())
	{
		
		PathPoints = HUDBase->GetPathReUseDijkstra( Units[i]->Next_DijkstraPMatrixes.Matrix, HitLocation, UnitLocation);
					
		if(PathPoints.Num())
		{
			Units[i]->RunLocation = PathPoints[0].Point;;
					
			for(int k = 1; k < PathPoints.Num(); k++)
			{
				Units[i]->RunLocationArray.Add(PathPoints[k].Point);
			}
						
			Units[i]->RunLocationArray.Add(HitLocation);
			Units[i]->FollowPath = true;
		}
	}else
	{
		Units[i]->FollowPath = false;
	}
}


void AControllerBase::SpacePressed()
{
	IsSpacePressed = true;
}

void AControllerBase::SpaceReleased()
{
	IsSpacePressed = false;
}


void AControllerBase::APressed()
{
	if(!AttackToggled)
	{
		AttackToggled = true;
		for (int32 i = 0; i < SelectedUnits.Num(); i++)
		{
			if (SelectedUnits[i] && SelectedUnits[i]->UnitState != UnitData::Dead)
			{
				if(SelectedUnits[i])
				SelectedUnits[i]->ToggleUnitDetection = true;
			}
		}
	}
}

void AControllerBase::AReleased()
{

}

void AControllerBase::JumpCamera()
{
	if (CameraBase && (IsStrgPressed || IsSpacePressed))
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
		CameraBase->JumpCamera(Hit);

	}
}


void AControllerBase::StrgPressed() {
	IsStrgPressed = true;
}

void AControllerBase::StrgReleased() {
	IsStrgPressed = false;
}

void AControllerBase::ZoomIn()
{
	if (CameraBase && IsStrgPressed)
	{
		CameraBase->ZoomIn();
	}
}

void AControllerBase::ZoomOut()
{
	if (CameraBase && IsStrgPressed)
	{
		CameraBase->ZoomOut();
	}
}
