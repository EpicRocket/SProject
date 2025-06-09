
#include "StageLevel.h"
// include Engine
#include "Engine/World.h"
// include Project
#include "StageLogging.h"
#include "Gameplay/Stage/ETC/StageBuildZone.h"
#include "Gameplay/Stage/ETC/StageSpawner.h"
#include "Gameplay/Stage/ETC/StageSupervisor.h"
#include "Gameplay/ETC/GameplayPathActor.h"
#include "StagePlayerPawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageLevel)

void AStageLevel::BeginPlay()
{
	Super::BeginPlay();

	OnInitailize();

	// TODO:
}

void AStageLevel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(Supervisor))
	{
		Supervisor->Destroy();
		Supervisor = nullptr;
	}

	PathActors.Empty();
	BuildZones.Empty();
	Spawners.Empty();
	PlayerPawn = nullptr;

	Super::EndPlay(EndPlayReason);
}

bool AStageLevel::ShouldShowLoadingScreen(FString& OutReason) const
{
	return false;
}

FGErrorInfo AStageLevel::Load()
{
	auto World = GetWorld();
	if (!World)
	{
		return GameCore::Throw(GameErr::WORLD_INVALID, FString::Printf(TEXT("%s:World is null"), *GetName()));
	}

	{
		FActorSpawnParameters Params;
		Params.Owner = this;

		Supervisor = World->SpawnActor<AStageSupervisor>(Params);
	}

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

	if (BuildZones.Contains(Position))
	{
	}

	BuildZonePtr->SourceStage = this;
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

void AStageLevel::SetPlayerPawn(AStagePlayerPawn* InPlayerPawn)
{
	PlayerPawn = InPlayerPawn;
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
