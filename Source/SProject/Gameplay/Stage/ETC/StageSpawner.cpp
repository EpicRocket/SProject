
#include "StageSpawner.h"
// include Engine
#include "Engine/World.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
// include Project
#include "Gameplay/ETC/GameplayPathActor.h"
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/StageMonsterTypes.h"
#include "Gameplay/Stage/Unit/StageUnitCharacter.h"

//AUnitCharacter* AStageSpawner::Spawn(const FStageSpawnParams& Params)
	/*if (!Params.StageLevel.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("StageLevel을 찾지 못하였습니다."));
		return nullptr;
	}

	auto PathActor = Params.StageLevel->GetPathActor(Params.PathPosition);
	if (!PathActor)
	{
		UE_LOG(LogStage, Warning, TEXT("PathActor을 찾지 못하였습니다."));
		return nullptr;
	}

	auto SpawnedUnit = UGameplayHelper::SpawnUnit(this, PathActor->GetActorLocation(), PathActor->GetActorRotation(), Params.SpawnUnit, AAIController::StaticClass());
	SpawnedUnit->SetGenericTeamId(GetGenericTeamId());

	return SpawnedUnit;*/

namespace Stage
{
	FGErrorInfo SpawnUnit(AStageLevel* StageLevel, FVector Location, FRotator Rotation, UClass* UnitClass, TFunction<void(AStageUnitCharacter*)> SpawningCallback, AStageUnitCharacter*& SpawnedUnit)
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

		auto SpawnUnit = World->SpawnActorDeferred<AStageUnitCharacter>(UnitClass, FTransform(Rotation, Location), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		SpawnUnit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
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

		return GameCore::Pass();
	}
}

FGErrorInfo AStageSpawner::SpawnMonster(const FStageMonsterSpawnParams& Params, AStageUnitCharacter*& SpawnedUnit)
{
	FStageMonsterInfo MonsterInfo;
	if (auto Err = UStageTableHelper::GetStageMonsterInfo(Params.Index, MonsterInfo); !GameCore::IsOK(Err))
	{
		return Err;
	}

	auto SpawningCallback = [&](AStageUnitCharacter* Unit)
		{
			// TODO: 정보
		};

	if (auto Err = Stage::SpawnUnit(Params.StageLevel.Get(), GetActorLocation(), GetActorRotation(), MonsterInfo.UnitClass, SpawningCallback, SpawnedUnit); !GameCore::IsOK(Err))
	{
		return Err;
	}

	return GameCore::Pass();
}
