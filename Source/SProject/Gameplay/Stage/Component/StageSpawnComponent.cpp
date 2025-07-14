// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageSpawnComponent.h"
// include Engine
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
// include Project
#include "Gameplay/Stage/Unit/StageUnitCharacter.h"
#include "Gameplay/Stage/AI/StageAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSpawnComponent)

FGErrorInfo UStageSpawnComponent::BeginSpawn(UClass* UnitClass, FVector Location, FRotator Rotation, TSubclassOf<AStageAIController> AIController, AStageUnitCharacter*& SpawningUnit)
{
	SpawningUnit = nullptr;

	auto World = GetWorld();
	if (!World)
	{
		return GameCore::Throw(GameErr::WORLD_INVALID, TEXT("BeginSpawn(SpawnedUnit)"));
	}

	SpawningUnit = World->SpawnActorDeferred<AStageUnitCharacter>(UnitClass, FTransform(Rotation, Location), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!SpawningUnit)
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("BeginSpawn(SpawnedUnit):Spawn failure."));
	}

	if (AIController)
	{
		SpawningUnit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
		SpawningUnit->AIControllerClass = AIController;
	}
	else
	{
		SpawningUnit->AutoPossessAI = EAutoPossessAI::Disabled;
	}

	return GameCore::Pass();
}

FGErrorInfo UStageSpawnComponent::EndSpawn(AStageUnitCharacter* SpawningUnit, FVector Location, FRotator Rotation)
{
	if(!IsValid(SpawningUnit))
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("EndSpawn(SpawningUnit):SpawningUnit is invalid."));
	}

	FVector SpawnLocation = Location;
	if (auto CharacterMovementComponent = SpawningUnit->GetCharacterMovement())
	{
		FNavLocation NavLocation;
		if (CharacterMovementComponent->FindNavFloor(SpawnLocation, NavLocation))
		{
			SpawnLocation = NavLocation.Location;
		}
		else
		{
			FFindFloorResult FloorHit;
			CharacterMovementComponent->FindFloor(SpawningUnit->GetActorLocation(), FloorHit, false);
			if (FloorHit.bWalkableFloor)
			{
				SpawnLocation = FloorHit.HitResult.ImpactPoint;
			}
		}
	}
	SpawningUnit->FinishSpawning(FTransform(Rotation, SpawnLocation), false, nullptr, ESpawnActorScaleMethod::MultiplyWithRoot);

	return GameCore::Pass();
}
