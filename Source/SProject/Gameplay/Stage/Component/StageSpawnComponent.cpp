
#include "StageSpawnComponent.h"
// include Engine
#include "Engine/World.h"
#include "AIController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
// include GameCore
#include "Core/Action/GGameComponentLoadAction.h"
// include Project
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Types/StageMonsterTypes.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"
#include "Gameplay/Stage/Unit/StageMonsterUnit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSpawnComponent)

namespace Stage
{
	template<typename T>
	FGErrorInfo SpawnUnit(uint8 TeamID, AStageLevel* StageLevel, FVector Location, FRotator Rotation, UClass* UnitClass, TFunction<void(T*)> SpawningCallback, T*& SpawnedUnit)
	{
		if (!IsValid(StageLevel))
		{
			return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("StageLevel"));
		}

		if (!UnitClass)
		{
			return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("UnitClass"));
		}

		auto World = StageLevel->GetWorld();
		if (!World)
		{
			return GameCore::Throw(GameErr::WORLD_INVALID);
		}

		auto SpawnUnit = World->SpawnActorDeferred<T>(UnitClass, FTransform(Rotation, Location), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		SpawnUnit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
		SpawnUnit->SetGenericTeamId(TeamID);
		if (SpawningCallback)
		{
			SpawningCallback(SpawnUnit);
		}

		FVector SpawnLocation = Location;

		if (auto CharacterMovementCom = SpawnUnit->GetCharacterMovement())
		{
			FNavLocation NavLocation;
			if (CharacterMovementCom->FindNavFloor(SpawnLocation, NavLocation))
			{
				SpawnLocation = NavLocation.Location;
			}
			else
			{
				FFindFloorResult FloorHit;
				CharacterMovementCom->FindFloor(SpawnUnit->GetActorLocation(), FloorHit, false);
				if (FloorHit.bWalkableFloor)
				{
					SpawnLocation = FloorHit.HitResult.ImpactPoint;
				}
			}
		}

		SpawnUnit->FinishSpawning(FTransform(Rotation, SpawnLocation), false, nullptr, ESpawnActorScaleMethod::MultiplyWithRoot);

		SpawnedUnit = SpawnUnit;
		
		return GameCore::Pass();
	}

	FGErrorInfo GetSpawnComponent(AStageLevel* StageLevel, UStageSpawnComponent*& SpawnComponent)
	{
		if (!IsValid(StageLevel))
		{
			return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("StageLevel"));
		}

		auto World = StageLevel->GetWorld();
		if (!World)
		{
			return GameCore::Throw(GameErr::WORLD_INVALID, TEXT("StageLevel"));
		}

		auto GameState = World->GetGameState();
		if (!GameState)
		{
			return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("GameState"));
		}

		SpawnComponent = GameState->GetComponentByClass<UStageSpawnComponent>();
		if (!SpawnComponent)
		{
			return GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("UStageSpawnComponent"));
		}

		return GameCore::Pass();
	}
}


//////////////////////////////////////////////////////////////////////////
// UStageSpawnComponent
//////////////////////////////////////////////////////////////////////////

FGErrorInfo UStageSpawnComponent::SpawnTower(uint8 TeamID, AStageLevel* StageLevel, FVector Location, FRotator Rotation, FStageTowerInfo TowerInfo, TSubclassOf<AAIController> AIController, AStageTowerUnit*& SpawnedUnit)
{
	auto SpawningCallback = [&](AStageTowerUnit* Unit)
		{
			Unit->Setup(TowerInfo);
			if (AIController)
			{
				Unit->AIControllerClass = AIController;
			}
		};

	if (auto Err = Stage::SpawnUnit<AStageTowerUnit>(TeamID, StageLevel, Location, Rotation, TowerInfo.UnitClass, SpawningCallback, SpawnedUnit); !GameCore::IsOK(Err))
	{
		return Err;
	}

	return GameCore::Pass();
}

FGErrorInfo UStageSpawnComponent::SpawnMonster(uint8 TeamID, AStageLevel* StageLevel, FVector Location, FRotator Rotation, FStageMonsterInfo MonsterInfo, TSubclassOf<AAIController> AIController, AStageMonsterUnit*& SpawnedUnit)
{
	auto SpawningCallback = [&](AStageMonsterUnit* Unit)
		{
			Unit->Setup(MonsterInfo);
			if (AIController)
			{
				Unit->AIControllerClass = AIController;
			}
		};

	if (auto Err = Stage::SpawnUnit<AStageMonsterUnit>(TeamID, StageLevel, Location, Rotation, MonsterInfo.UnitClass, SpawningCallback, SpawnedUnit); !GameCore::IsOK(Err))
	{
		return Err;
	}

	return GameCore::Pass();
}

//////////////////////////////////////////////////////////////////////////
// UStageSpawnHelper
//////////////////////////////////////////////////////////////////////////

/* static */FGErrorInfo UStageSpawnHelper::SpawnTower(uint8 TeamID, AStageLevel* StageLevel, FVector Location, FRotator Rotation, FStageTowerInfo TowerInfo, TSubclassOf<AAIController> AIController, AStageTowerUnit*& SpawnedUnit)
{
	UStageSpawnComponent* SpawnComponent = nullptr;
	if (auto Err = Stage::GetSpawnComponent(StageLevel, SpawnComponent); !GameCore::IsOK(Err))
	{
		return Err;
	}

	return SpawnComponent->SpawnTower(TeamID, StageLevel, Location, Rotation, TowerInfo, AIController, SpawnedUnit);
}

/* static */FGErrorInfo UStageSpawnHelper::SpawnMonster(uint8 TeamID, AStageLevel* StageLevel, FVector Location, FRotator Rotation, FStageMonsterInfo MonsterInfo, TSubclassOf<AAIController> AIController, AStageMonsterUnit*& SpawnedUnit)
{
	UStageSpawnComponent* SpawnComponent = nullptr;
	if (auto Err = Stage::GetSpawnComponent(StageLevel, SpawnComponent); !GameCore::IsOK(Err))
	{
		return Err;
	}

	return SpawnComponent->SpawnMonster(TeamID, StageLevel, Location, Rotation, MonsterInfo, AIController, SpawnedUnit);
}
