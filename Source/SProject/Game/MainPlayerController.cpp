
#include "MainPlayerController.h"
#include "System/SuperGameUserSettings.h"
#include "MainInputComponent.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void AMainPlayerController::SetupInputComponent()
{
	if (!IsValid(MainInputComponent))
	{
		MainInputComponent = NewObject<UMainInputComponent>(this, UMainInputComponent::StaticClass(), TEXT("PC_MainInputComponent0"));
	}
	InputComponent = MainInputComponent;

	Super::SetupInputComponent();
}
