// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageSpawner.h"
// include Engine

// include Project


FVector AStageSpawner::GetSpawnLocation() const
{
	// TODO: 스폰 위치 랜덤성
	return GetActorLocation();
}

FRotator AStageSpawner::GetSpawnRotation() const
{
	// TODO: 스폰 각도
	return GetActorRotation();
}

FGErrorInfo AStageSpawner::SpawnMonster(const FStageMonsterSpawnParams& Params, AStageMonsterUnit*& SpawnedUnit)
{
	/*UStageSpawnComponent* SpawnComponent = nullptr;
	if (auto Err = GetSpawnComponent(SpawnComponent); !GameCore::IsOK(Err))
	{
		return Err;
	}

	FStageMonsterInfo Info;
	if (auto Err = UStageTableHelper::GetStageMonsterInfo(Params.Index, Info); !GameCore::IsOK(Err))
	{
		return Err;
	}*/

	/*if (auto Err = SpawnComponent->SpawnMonster(GetTeamID(), Params.StageLevel.Get(), GetActorLocation(), GetActorRotation(), Info, nullptr, SpawnedUnit); !GameCore::IsOK(Err))
	{
		return Err;
	}

	if (auto AIController = SpawnedUnit->GetController<AStageAIController>())
	{
		AIController->SetGenericTeamId(GetTeamID());
		AIController->SourceStage = Params.StageLevel;
		AIController->AIBehaviorTree = Info.AI;
		if (Params.PathPosition != INDEX_NONE)
		{
			UGameplayHelper::SetGameplayTagByInt32(AIController, AGameplayPathActor::PathTagName, Params.PathPosition);
		}
		
		AIController->StartAI();
	}*/

	return GameCore::Pass();
}

FGErrorInfo AStageSpawner::GetSpawnComponent(UStageSpawnComponent*& SpawnComponent) const
{
	/*auto World = GetWorld();
	if (!World)
	{
		return GameCore::Throw(GameErr::WORLD_INVALID);
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
	}*/

	return GameCore::Pass();
}
