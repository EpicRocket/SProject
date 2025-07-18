// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageLevel.h"
// include Engine
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
// include GGameCore
#include "Core/GGameCoreHelper.h"
// include Project
#include "Gameplay/Stage/Stage.h"
#include "Gameplay/Stage/ETC/StageBuildZone.h"
#include "Gameplay/Stage/ETC/StageSpawner.h"
#include "Gameplay/Stage/ETC/StageSupervisor.h"
#include "Gameplay/ETC/GameplayPathActor.h"
#include "StagePlayerPawn.h"
#include "Types/StageTypes.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(StageLevel)

void AStageLevel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(Supervisor))
	{
		Supervisor->Destroy();
		Supervisor = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

bool AStageLevel::ShouldShowLoadingScreen(FString& OutReason) const
{
	return false;
}

FGErrorInfo AStageLevel::Setup(int32 InStageLevel, TSubclassOf<AStageSupervisor> InSupervisorClass)
{
	StageLevel = InStageLevel;

	auto World = GetWorld();
	if (!World)
	{
		return GameCore::Throw(GameErr::WORLD_INVALID, FString::Printf(TEXT("AStageLevel::Setup(InStageLevel: %d, InSupervisorClass: %s):월드를 찾을 수 없습니다."), InStageLevel, InSupervisorClass ? TEXT("Exist") : TEXT("NotExist")));
	}

	if (!InSupervisorClass)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("AStageLevel::Setup(InStageLevel: %d, InSupervisorClass: %s):SupervisorClass를 찾을 수 없습니다."), InStageLevel));
	}

	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Supervisor = World->SpawnActor<AStageSupervisor>(InSupervisorClass, Params);
	}

	OnInitailize();

	return GameCore::Pass();
}

void AStageLevel::AddBuildZone(AStageBuildZone* BuildZone)
{
	TWeakObjectPtr<AStageBuildZone> BuildZonePtr = BuildZone;

	if (!BuildZonePtr.IsValid())
	{
		return;
	}

	if (BuildZonePtr->Tags.IsEmpty())
	{
		return;
	}

	int32 Position = BuildZone->GetPosition();
	if (Position == INDEX_NONE)
	{
		return;
	}

	BuildZone->Setup(Supervisor);
	BuildZones.Emplace(Position, BuildZonePtr);
}

AStageBuildZone* AStageLevel::GetBuildZone(int32 Position) const
{
	if (!BuildZones.Contains(Position))
	{
		return nullptr;
	}

	auto& BuildZone = BuildZones[Position];
	if (!BuildZone.IsValid())
	{
		return nullptr;
	}

	return BuildZone.Get();
}

TArray<AStageBuildZone*> AStageLevel::GetBuildZones() const
{
	TArray<AStageBuildZone*> Result;

	for (auto& [_, BuildZone] : BuildZones)
	{
		if (BuildZone.IsValid())
		{
			Result.Emplace(BuildZone.Get());
		}
	}

	return Result;
}

void AStageLevel::SetPlayerPawn(AStagePlayerPawn* InPlayerPawn)
{
	PlayerPawn = InPlayerPawn;
}

APawn* AStageLevel::GetPlayerPawn() const
{
	if (!PlayerPawn.IsValid())
	{
		GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("[AStageLevel::GetPlayerPawn]PlayerPawn is not valid"));
		return nullptr;
	}

	return PlayerPawn.Get();
}

void AStageLevel::AddPathActor(AGameplayPathActor* PathActor)
{
	TWeakObjectPtr<AGameplayPathActor> PathActorPtr = PathActor;

	if (!PathActorPtr.IsValid())
	{
		return;
	}

	if (PathActorPtr->Tags.IsEmpty())
	{
		return;
	}

	int32 Position = PathActor->GetPosition();
	if (Position == INDEX_NONE)
	{
		return;
	}

	if (PathActors.Contains(Position))
	{
	}

	PathActors.Emplace(Position, PathActorPtr);
}

AGameplayPathActor* AStageLevel::GetPathActor(int32 Position) const
{
	if (!PathActors.Contains(Position))
	{
		return nullptr;
	}

	auto& PathActor = PathActors[Position];
	if (!PathActor.IsValid())
	{
		return nullptr;
	}

	return PathActor.Get();
}

TArray<AGameplayPathActor*> AStageLevel::GetPathActors() const
{
	TArray<AGameplayPathActor*> Result;

	for (auto& [_, PathActor] : PathActors)
	{
		if (PathActor.IsValid())
		{
			Result.Emplace(PathActor.Get());
		}
	}

	return Result;
}

void AStageLevel::AddSpawner(AStageSpawner* Spawner)
{
	TWeakObjectPtr<AStageSpawner> SpawnerPtr = Spawner;
	if (!SpawnerPtr.IsValid())
	{
		return;
	}
	if (SpawnerPtr->Tags.IsEmpty())
	{
		return;
	}
	int32 Position = Spawner->GetPosition();
	if (Position == INDEX_NONE)
	{
		return;
	}
	if (Spawners.Contains(Position))
	{
	}
	Spawners.Emplace(Position, SpawnerPtr);
}

AStageSpawner* AStageLevel::GetSpawner(int32 Position) const
{
	if (!Spawners.Contains(Position))
	{
		return nullptr;
	}

	auto& Spawner = Spawners[Position];

	if (!Spawner.IsValid())
	{
		return nullptr;
	}

	return Spawner.Get();
}

TArray<AStageSpawner*> AStageLevel::GetSpawners() const
{
	TArray<AStageSpawner*> Result;

	for (auto& [_, Spawner] : Spawners)
	{
		if (Spawner.IsValid())
		{
			Result.Emplace(Spawner.Get());
		}
	}

	return Result;
}

void AStageLevel::OnInitailize()
{
	TArray<AActor*> FindActors;

	GetActorsByClass(AGameplayPathActor::StaticClass(), FindActors);
	for (auto Actor : FindActors)
	{
		AddPathActor(Cast<AGameplayPathActor>(Actor));
	}

	GetActorsByClass(AStageBuildZone::StaticClass(), FindActors);
	for (auto Actor : FindActors)
	{
		AddBuildZone(Cast<AStageBuildZone>(Actor));
	}

	GetActorsByClass(AStageSpawner::StaticClass(), FindActors);
	for (auto Actor : FindActors)
	{
		AddSpawner(Cast<AStageSpawner>(Actor));
	}

	GetActorsByClass(AStagePlayerPawn::StaticClass(), FindActors);
	for (auto Actor : FindActors)
	{
		auto Pawn = Cast<AStagePlayerPawn>(Actor);
		if (!Pawn)
		{
			continue;
		}
		SetPlayerPawn(Pawn);
		break;
	}
}
