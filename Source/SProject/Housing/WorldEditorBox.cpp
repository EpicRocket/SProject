#include "WorldEditorBox.h"

#include "Components/LineBatchComponent.h"

#include "WorldEditorComponent.h"


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
}

void AWorldEditorBox::PreInitializeComponents()
{
	Super::PreInitializeComponents();

}

void AWorldEditorBox::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	//FColor BoxColor = FColor::Red;
	//DrawDebugBox(GetWorld(), GetActorLocation() + FVector(0.0F, 0.0F, 25.0F), FVector(500.0F, 500.0F, 25.0F), FQuat::Identity, BoxColor, true, -1.0f, 0, 2.0f);

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

