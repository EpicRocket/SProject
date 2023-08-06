// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppMemberFunctionMayBeStatic
#include "TerritoryPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "TerritoryDefine.h"
#include "TerritoryBuilding.h"
#include "System/SuperFunctionLibrary.h"

ATerritoryPlayerController::ATerritoryPlayerController()
{	
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	
	bEnableTouchEvents = true;
	bEnableTouchOverEvents = true;
}

void ATerritoryPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// if(const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	// {
	// 	SubSystem->ClearAllMappings();
	// 	SubSystem->AddMappingContext(InputMapping, 0);
	// }
	//
	// if(const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	// {
	// 	if(InputConfigData)
	// 	{
	// 		EnhancedInputComponent->BindAction(InputConfigData->InputPick, ETriggerEvent::Triggered, this, &ATerritoryPlayerController::OnTouchBegin);
	// 		EnhancedInputComponent->BindAction(InputConfigData->InputPick, ETriggerEvent::Triggered, this, &ATerritoryPlayerController::OnHold);
	// 		EnhancedInputComponent->BindAction(InputConfigData->InputPick, ETriggerEvent::Triggered, this, &ATerritoryPlayerController::OnTouchEnd);
	// 		EnhancedInputComponent->BindAction(InputConfigData->InputPick, ETriggerEvent::Triggered, this, &ATerritoryPlayerController::OnTap);
	// 	}
	// }
}

void ATerritoryPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PreviewBuilding = GetWorld()->SpawnActor<AActor>();
	PreviewBuilding->SetRootComponent(NewObject<USceneComponent>(PreviewBuilding));
	PreviewBuilding->SetActorHiddenInGame(true);
}

void ATerritoryPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void ATerritoryPlayerController::RegisterBuilding(TSubclassOf<ATerritoryBuilding> InBuildingBP)
{	
	BuildingBP = InBuildingBP;
	TSubclassOf<AActor> A = BuildingBP;
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	USuperFunctionLibrary::GetComponents(BuildingBP, StaticMeshComponents);
	for(const auto Mesh : StaticMeshComponents)
	{
		const TObjectPtr<UStaticMeshComponent> PreviewMesh = NewObject<UStaticMeshComponent>(PreviewBuilding, Mesh->GetFName(), RF_NoFlags, Mesh);
		PreviewMesh->AttachToComponent(PreviewBuilding->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		PreviewMesh->SetRelativeTransform(Mesh->GetRelativeTransform());
		PreviewMesh->RegisterComponent();
	}
	
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	USuperFunctionLibrary::GetComponents(BuildingBP, SkeletalMeshComponents);
	for(const auto Mesh : SkeletalMeshComponents)
	{
		const TObjectPtr<USkeletalMeshComponent> PreviewMesh = NewObject<USkeletalMeshComponent>(PreviewBuilding, Mesh->GetFName(), RF_NoFlags, Mesh);
		PreviewMesh->AttachToComponent(PreviewBuilding->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		PreviewMesh->SetRelativeTransform(Mesh->GetRelativeTransform());
		PreviewMesh->RegisterComponent();
	}
}

void ATerritoryPlayerController::UnRegisterBuilding()
{
	auto Components = PreviewBuilding->GetComponents();
	for(auto Component : Components)
	{
		if(PreviewBuilding->GetRootComponent() == Component)
		{
			continue;
		}
		Component->UnregisterComponent();
		Component->DestroyComponent();
	}
}

void ATerritoryPlayerController::SetPreviewBuildingVisiblity(bool bEnabled)
{
	PreviewBuilding->SetActorHiddenInGame(!bEnabled);
}

void ATerritoryPlayerController::SetModeType(const ETerritoryModeType InModeType)
{
	ModeType = InModeType;
	switch (ModeType)
	{
	case ETerritoryModeType::Idle:
		CurrentClickTraceChannel = ECC_Visibility;
		break;
	case ETerritoryModeType::Construct:
		CurrentClickTraceChannel = ECC_GameTraceChannel1;
		break;
	case ETerritoryModeType::Move:
		CurrentClickTraceChannel = ECC_GameTraceChannel1;
		break;
	}	
}

void ATerritoryPlayerController::OnTouchBegin(const FInputActionValue& ActionValue)
{
}

void ATerritoryPlayerController::OnHold(const FInputActionValue& ActionValue)
{
}

void ATerritoryPlayerController::OnTouchEnd(const FInputActionValue& ActionValue)
{
}

void ATerritoryPlayerController::OnTap(const FInputActionValue& ActionValue)
{
	
}

void ATerritoryPlayerController::OnPan(const FInputActionValue& ActionValue)
{
	FVector Offset = ActionValue.Get<FVector>();
	GetPawn()->AddMovementInput(Offset);
}

void ATerritoryPlayerController::StartPan()
{
	StartPanPos = GetPawn()->GetActorLocation();
}
