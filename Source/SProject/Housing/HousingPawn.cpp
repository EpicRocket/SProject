#include "HousingPawn.h"
#include "WorldEditorBox.h"

#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"



FName AHousingPawn::RootName = TEXT("RootComponent");
FName AHousingPawn::UserSceneComponentName = TEXT("UserSceneComponent");
FName AHousingPawn::HousingMovementComponentName = TEXT("HousingMovementComponent");

AHousingPawn::AHousingPawn()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(RootName);

	UserSceneComponent = CreateDefaultSubobject<USceneComponent>(UserSceneComponentName);
	UserSceneComponent->SetupAttachment(RootComponent);

	HousingMovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(HousingMovementComponentName);
	HousingMovementComponent->SetUpdatedComponent(RootComponent);

	//PrimaryActorTick.bCanEverTick = true;

}

void AHousingPawn::SetWorldEditorBox(class AWorldEditorBox* WorldEditorBox)
{
	MyWorldEditorBox = WorldEditorBox;
}

void AHousingPawn::SetFreedom(bool bActive)
{
	bFreedom = bActive;
}

void AHousingPawn::SetPosition(FVector WorldPosition)
{
	if (!MyWorldEditorBox)
	{
		return;
	}

	SetActorLocation(MyWorldEditorBox->WorldToBoxPosition(WorldPosition));
}

void AHousingPawn::AddMovementAxis(FVector Axis)
{
	UPawnMovementComponent* MovementComponent = GetMovementComponent();
	if (nullptr == MovementComponent)
	{
		return;
	}

	if (MyWorldEditorBox)
	{
		if (bFreedom)
		{
			FVector Result{ 0.0F, 0.0F, 0.0F };

			if (Axis.Y > 0)
			{
				Result = UserSceneComponent->GetForwardVector();
			}
			else if (Axis.Y < 0)
			{
				Result = -UserSceneComponent->GetForwardVector();
			}

			if (Axis.X > 0)
			{
				Result += UserSceneComponent->GetRightVector();
			}
			else if (Axis.X < 0)
			{
				Result -= UserSceneComponent->GetRightVector();
			}

			FVector NewPosition = GetActorLocation() + MovementComponent->Velocity * GetWorld()->GetDeltaSeconds();

			if (MyWorldEditorBox->IsInWorld(NewPosition))
			{
				MovementComponent->AddInputVector(Result, true);
			}
			else
			{
				FVector CenterDirection = (MyWorldEditorBox->GetActorLocation() - GetActorLocation()).GetUnsafeNormal();
				MovementComponent->AddInputVector(CenterDirection, true);

				// Calculate angle between Result and CenterDirection 
				float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Result, CenterDirection) / (Result.Size() * CenterDirection.Size())));
				if (Angle <= 90.0F)
				{
					MovementComponent->AddInputVector(Result, true);
				}
			}
		}
		else
		{
			FVector NewPosition = MyWorldEditorBox->WorldAndAxisToBoxPosition(GetActorLocation(), FIntVector(Axis.X, -Axis.Y, Axis.Z));
			TeleportTo(NewPosition, GetActorRotation(), false, false);
		}
	}
	else
	{
		// Missing World...
	}
}
