
#include "StageLevel.h"
// include Project
#include "StageLogging.h"
#include "Gameplay/Stage/ETC/StageBuildZone.h"
#include "Gameplay/Stage/ETC/StageSpawner.h"
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
	// TODO:
	BuildZones.Empty();

	Super::EndPlay(EndPlayReason);
}

bool AStageLevel::ShouldShowLoadingScreen(FString& OutReason) const
{
	return false;
}

void AStageLevel::AddBuildZone(AStageBuildZone* BuildZone)
{
	TWeakObjectPtr<AStageBuildZone> BuildZonePtr = BuildZone;

	if (!BuildZonePtr.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone을 찾지 못하였습니다."));
		return;
	}

	if (BuildZonePtr->Tags.IsEmpty())
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone(%s)에 부여된 태그가 없습니다."), *BuildZonePtr->GetFName().ToString());
		return;
	}

	int32 Position = BuildZone->GetPosition();
	if (Position == INDEX_NONE)
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone(%s)에 Position 태그가 없습니다."), *BuildZonePtr->GetFName().ToString());
		return;
	}

	if (BuildZones.Contains(Position))
	{
		UE_LOG(LogStage, Warning, TEXT("이미 BuildZone이 존재합니다. 기존 BuildZone이 덮어집니다. Position(%d)"), Position);
	}

	BuildZones.Emplace(Position, BuildZonePtr);
}

AStageBuildZone* AStageLevel::GetBuildZone(int32 Position) const
{
	if (!BuildZones.Contains(Position))
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone을 찾지 못하였습니다. [Level: %s][Position: %d]"), *GetFName().ToString(), Position);
		return nullptr;
	}

	auto& BuildZone = BuildZones[Position];
	if (!BuildZone.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone이 유효하지 않습니다. [Level: %s][Position: %d]"), *GetFName().ToString(), Position);
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
		UE_LOG(LogStage, Warning, TEXT("PathActor을 찾지 못하였습니다."));
		return;
	}

	if (PathActorPtr->Tags.IsEmpty())
	{
		UE_LOG(LogStage, Warning, TEXT("PathActor(%s)에 부여된 태그가 없습니다."), *PathActorPtr->GetFName().ToString());
		return;
	}

	int32 Position = PathActor->GetPosition();
	if (Position == INDEX_NONE)
	{
		UE_LOG(LogStage, Warning, TEXT("PathActor(%s)에 Position 태그가 없습니다."), *PathActorPtr->GetFName().ToString());
		return;
	}

	if (PathActors.Contains(Position))
	{
		UE_LOG(LogStage, Warning, TEXT("이미 PathActor이 존재합니다. 기존 PathActor이 덮어집니다. Position(%d)"), Position);
	}

	PathActors.Emplace(Position, PathActorPtr);
}

AGameplayPathActor* AStageLevel::GetPathActor(int32 Position) const
{
	if (!PathActors.Contains(Position))
	{
		UE_LOG(LogStage, Warning, TEXT("PathActor을 찾지 못하였습니다. [Level: %s][Position: %d]"), *GetFName().ToString(), Position);
		return nullptr;
	}

	auto& PathActor = PathActors[Position];
	if (!PathActor.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("PathActor이 유효하지 않습니다. [Level: %s][Position: %d]"), *GetFName().ToString(), Position);
		return nullptr;
	}

	return PathActor.Get();
}

void AStageLevel::AddSpawner(AStageSpawner* Spawner)
{
	TWeakObjectPtr<AStageSpawner> SpawnerPtr = Spawner;
	if (!SpawnerPtr.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("Spawner을 찾지 못하였습니다."));
		return;
	}
	if (SpawnerPtr->Tags.IsEmpty())
	{
		UE_LOG(LogStage, Warning, TEXT("Spawner(%s)에 부여된 태그가 없습니다."), *SpawnerPtr->GetFName().ToString());
		return;
	}
	int32 Position = Spawner->GetPosition();
	if (Position == INDEX_NONE)
	{
		UE_LOG(LogStage, Warning, TEXT("Spawner(%s)에 Position 태그가 없습니다."), *SpawnerPtr->GetFName().ToString());
		return;
	}
	if (Spawners.Contains(Position))
	{
		UE_LOG(LogStage, Warning, TEXT("이미 Spawner이 존재합니다. 기존 Spawner이 덮어집니다. Position(%d)"), Position);
	}
	Spawners.Emplace(Position, SpawnerPtr);
}

AStageSpawner* AStageLevel::GetSpawner(int32 Position) const
{
	if (!Spawners.Contains(Position))
	{
		UE_LOG(LogStage, Warning, TEXT("Spawner을 찾지 못하였습니다. [Level: %s][Position: %d]"), *GetFName().ToString(), Position);
		return nullptr;
	}

	auto& Spawner = Spawners[Position];

	if (!Spawner.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("Spawner이 유효하지 않습니다. [Level: %s][Position: %d]"), *GetFName().ToString(), Position);
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
		// FIXME: 이것도 배열로 만들어서 관리해도 좋을듯
		auto Pawn = Cast<AStagePlayerPawn>(Actor);
		if (!Pawn)
		{
			continue;
		}
		SetPlayerPawn(Pawn);
		break;
	}
}
