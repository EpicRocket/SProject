#include "EnvironmentProp.h"

#if WITH_EDITOR

#include "Kismet/KismetSystemLibrary.h"

#endif


FName AEnvironmentProp::RootName = TEXT("RootComponent");

AEnvironmentProp::AEnvironmentProp()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(RootName);

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


#if WITH_EDITOR

void AEnvironmentProp::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	
}

#endif