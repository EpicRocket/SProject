
#include "SProjectCamera.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "UnrealClient.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SProjectCamera)

ASProjectCamera::ASProjectCamera(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ASProjectCamera::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine && GEngine->GameViewport)
	{
		FViewport* Viewport = GEngine->GameViewport->Viewport;
		if (Viewport)
		{
			ScreenSizeX = Viewport->GetSizeXY().X;
			ScreenSizeY = Viewport->GetSizeXY().Y;

			// Viewport 사이즈 출력
			UE_LOG(LogTemp, Warning, TEXT("Viewport Size: Width = %d, Height = %d"), ScreenSizeX, ScreenSizeY);
		}
	}
}

void ASProjectCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	float MousePositionX = 0.0F;
	float MousePositionY = 0.0F;
	
	float DirectionX = 0.0F;
	float DirectionY = 0.0F;


	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerControllerFromID(this, 0);
	if (!IsValid(LocalPlayerController))
	{
		return;
	}

	if (!LocalPlayerController->GetMousePosition(MousePositionX, MousePositionY))
	{
		return;
	}
	
	if (MousePositionX <= Margin)
	{
		DirectionY = -1.0F;
	}
	if (MousePositionY <= Margin)
	{
		DirectionX = 1.0F;
	}

	if (MousePositionX >= ScreenSizeX - Margin)
	{
		DirectionY = 1.0F;
	}
	if (MousePositionY >= ScreenSizeY - Margin)
	{
		DirectionX = -1.0F;
	}

	FVector Direction(DirectionX, DirectionY, 0.0F);
	Direction *= ScrollSpeed;

	if (FMath::IsNearlyZero(Direction.SizeSquared()))
	{
		return;
	}

	AddActorWorldOffset(Direction);
}
