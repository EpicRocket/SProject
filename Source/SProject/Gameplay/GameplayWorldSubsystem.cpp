
#include "GameplayWorldSubsystem.h"
// includ Engine
#include "Engine/Engine.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
// include Project
#include "Unit/UnitCharacter.h"


#include "GameplayLogging.h"
// C:\UnrealEngine\Engine\Source\Runtime\NavigationSystem\Public\NavigationSystem.h

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayWorldSubsystem)

AUnitCharacter* UGameplayHelper::SpawnUnit(UObject* WorldContextObject, FVector Location, FRotator Rotation, TSubclassOf<AUnitCharacter> UnitClass, TSubclassOf<AAIController> AIController)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	if (!UnitClass)
	{
		return nullptr;
	}

	AUnitCharacter* SpawnUnit = World->SpawnActorDeferred<AUnitCharacter>(UnitClass, FTransform(Rotation, Location), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	SpawnUnit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	SpawnUnit->AIControllerClass = AIController;

	FVector AdjustedLocation = Location;
	bool bFind = false;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (NavSys)
	{
		float Radius = SpawnUnit->GetCapsuleComponent()->GetScaledCapsuleRadius();
		FNavLocation NavAdjustedLocation;
		if (NavSys->ProjectPointToNavigation(Location, NavAdjustedLocation, FVector(Radius, Radius, Radius)))
		{
			AdjustedLocation = NavAdjustedLocation.Location;
			bFind = true;
		}
	}

	if (!bFind)
	{
		FHitResult Result;
		FVector EndLocation = Location - FVector(0.0f, 0.0f, 1000.0F); // 아래 방향으로 Raycast

		bool bHit = World->LineTraceSingleByChannel(Result, Location, EndLocation, ECollisionChannel::ECC_WorldStatic);
		if (bHit)
		{
			AdjustedLocation = Result.ImpactPoint;
		}
	}

	float HalfHeight = SpawnUnit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SpawnUnit->FinishSpawning(FTransform(Rotation, AdjustedLocation + HalfHeight), false, nullptr, ESpawnActorScaleMethod::MultiplyWithRoot);

	return SpawnUnit;
}
