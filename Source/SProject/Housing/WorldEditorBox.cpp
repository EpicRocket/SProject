#include "WorldEditorBox.h"

#include "Components/LineBatchComponent.h"

#include "WorldEditorComponent.h"
#include "EnvironmentProp.h"

#if WITH_EDITOR

#include "Editor/EditorEngine.h"
#include "Engine/Selection.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

/** The editor object. */
extern UNREALED_API class UEditorEngine* GEditor;
#endif

#if WITH_EDITOR

namespace
{

void CalculatePlace(TObjectPtr<AWorldEditorBox> WorldEditorBox, TObjectPtr<AEnvironmentProp> EnvironmentProp)
{
	if (!WorldEditorBox || !EnvironmentProp)
	{
		return;
	}

	FVector NewPosition = WorldEditorBox->WorldToBoxPosition(EnvironmentProp->GetActorLocation());

	FVector ActorOrigin;
	FVector ActorExtent;
	EnvironmentProp->GetActorBounds(false, ActorOrigin, ActorExtent);
	DrawDebugSolidBox(WorldEditorBox->GetWorld(), NewPosition + FVector(0.0F, 0.0F, ActorExtent.Z), ActorExtent, WorldEditorBox->GetActorRotation().Quaternion(), FColor(255, 0, 0, 128), false, 0.0F);
}

}

#endif


//! AWorldEditorBox

FName AWorldEditorBox::RootName = TEXT("RootComponent");
FName AWorldEditorBox::WorldEditorComponentName = TEXT("WorldEditorComponent");

AWorldEditorBox::AWorldEditorBox()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(RootName);

	WorldEditorComponent = CreateDefaultSubobject<UWorldEditorComponent>(WorldEditorComponentName);
	WorldEditorComponent->SetupAttachment(RootComponent);

	//! Default Settgins
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
	bAsyncPhysicsTickEnabled = false;

	bNetLoadOnClient = false;

	// will be updated in PostInitProperties
	SetCanBeDamaged(false);

#if WITH_EDITOR

	if (GEditor && !GEditor->PlayWorld)
	{
		DebugSelectedProp = nullptr;
		GEditor->OnActorMoving().AddUObject(this, &AWorldEditorBox::OnActorMoving);
		GEditor->OnActorMoved().AddUObject(this, &AWorldEditorBox::OnActorMoved);
	}

#endif
}

void AWorldEditorBox::PreInitializeComponents()
{
	Super::PreInitializeComponents();

}

void AWorldEditorBox::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	if (TickType != LEVELTICK_ViewportsOnly)
	{
		return;
	}

	if (DebugSelectedProp)
	{
		CalculatePlace(this, DebugSelectedProp);
	}
}

void AWorldEditorBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (WorldEditorComponent)
	{
		WorldEditorComponent->DrawOctree();
	}
}

FVector AWorldEditorBox::WorldToBoxPosition(FVector WorldPosition)
{
	FVector RelativeLocation = WorldPosition - GetActorLocation();
	const FVector HalfExtent = WorldEditorComponent->GetExtent() / 2.0F;
	const float GridSize = WorldEditorComponent->GetGridSize();

	const FVector MinPosition = FVector(-HalfExtent.X, -HalfExtent.Y, 0.0F);
	const FVector MaxPosition = FVector(HalfExtent.X, HalfExtent.Y, HalfExtent.Z * 2.0F);

	// Calculate approximations position
	RelativeLocation += FVector(GridSize / 2.0F, GridSize / 2.0F, 0.0F);
	RelativeLocation /= GridSize;
	RelativeLocation.X = FMath::RoundToInt(RelativeLocation.X);
	RelativeLocation.Y = FMath::RoundToInt(RelativeLocation.Y);
	RelativeLocation.Z = FMath::RoundToInt(RelativeLocation.Z);
	RelativeLocation *= GridSize;

	RelativeLocation.X = FMath::Clamp(RelativeLocation.X, MinPosition.X, MaxPosition.X);
	RelativeLocation.Y = FMath::Clamp(RelativeLocation.Y, MinPosition.Y, MaxPosition.Y);
	RelativeLocation.Z = FMath::Clamp(RelativeLocation.Z, MinPosition.Z, MaxPosition.Z);

	RelativeLocation -= FVector(GridSize / 2.0F, GridSize / 2.0F, 0.0F);
	RelativeLocation += GetActorLocation();

	return RelativeLocation;
}

FVector AWorldEditorBox::WorldAndAxisToBoxPosition(FVector WorldPosition, FIntVector Axis)
{
	FVector RelativeLocation = WorldPosition - GetActorLocation();
	const FVector HalfExtent = WorldEditorComponent->GetExtent() / 2.0F;
	const float GridSize = WorldEditorComponent->GetGridSize();

	const FVector MinPosition = FVector(-HalfExtent.X + GridSize, -HalfExtent.Y + GridSize, 0.0F);
	const FVector MaxPosition = FVector(HalfExtent.X, HalfExtent.Y, HalfExtent.Z * 2.0F);

	// Calculate approximations position
	RelativeLocation += FVector(GridSize / 2.0F, GridSize / 2.0F, 0.0F);
	RelativeLocation /= GridSize;
	RelativeLocation.X = FMath::RoundToInt(RelativeLocation.X) + Axis.X;
	RelativeLocation.Y = FMath::RoundToInt(RelativeLocation.Y) + Axis.Y;
	RelativeLocation.Z = FMath::RoundToInt(RelativeLocation.Z) + Axis.Z;
	RelativeLocation *= GridSize;

	RelativeLocation.X = FMath::Clamp(RelativeLocation.X, MinPosition.X, MaxPosition.X);
	RelativeLocation.Y = FMath::Clamp(RelativeLocation.Y, MinPosition.Y, MaxPosition.Y);
	RelativeLocation.Z = FMath::Clamp(RelativeLocation.Z, MinPosition.Z, MaxPosition.Z);

	RelativeLocation -= FVector(GridSize / 2.0F, GridSize / 2.0F, 0.0F);
	RelativeLocation += GetActorLocation();

	return RelativeLocation;
}

bool AWorldEditorBox::IsInWorld(FVector WorldPosition) const
{
	return WorldEditorComponent->IsInWorld(WorldPosition - GetActorLocation());
}

void AWorldEditorBox::BeginPlay()
{
	Super::BeginPlay();

}

#if WITH_EDITOR

void AWorldEditorBox::OnActorMoving(AActor* SelcetedActor)
{
	TObjectPtr<AEnvironmentProp> EnvironmentProp = Cast<AEnvironmentProp>(SelcetedActor);
	if (!EnvironmentProp)
	{
		return;
	}
	DebugSelectedProp = EnvironmentProp;
}

void AWorldEditorBox::OnActorMoved(AActor* SelcetedActor)
{
	if (!DebugSelectedProp)
	{
		return;
	}
	FVector NewPosition = WorldToBoxPosition(DebugSelectedProp->GetActorLocation());
	DebugSelectedProp->SetActorLocation(NewPosition);
	DebugSelectedProp = nullptr;
}

#endif

