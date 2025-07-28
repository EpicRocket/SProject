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
#include "Gameplay/Stage/ETC/StageStartPoint.h"
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
	Supervisor->Setup(this);

	return GameCore::Pass();
}

void AStageLevel::AddBuildZone(AStageBuildZone* BuildZone)
{
	TWeakObjectPtr<AStageBuildZone> ObjectPtr = BuildZone;

	if (!ObjectPtr.IsValid())
	{
		return;
	}

	if (ObjectPtr->Tags.IsEmpty())
	{
		return;
	}

	int32 Position = ObjectPtr->GetPosition();
	if (Position == INDEX_NONE)
	{
		return;
	}

	ObjectPtr->Setup(Supervisor);
	BuildZones.Emplace(Position, ObjectPtr);
}

AStageBuildZone* AStageLevel::GetBuildZone(int32 Position) const
{
	if (!BuildZones.Contains(Position))
	{
		return nullptr;
	}

	auto& Element = BuildZones[Position];
	if (!Element.IsValid())
	{
		return nullptr;
	}

	return Element.Get();
}

TArray<AStageBuildZone*> AStageLevel::GetBuildZones() const
{
	TArray<AStageBuildZone*> Result;

	for (auto& [_, Element] : BuildZones)
	{
		if (Element.IsValid())
		{
			Result.Emplace(Element.Get());
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
	TWeakObjectPtr<AGameplayPathActor> ObjectPtr = PathActor;

	if (!ObjectPtr.IsValid())
	{
		return;
	}

	if (ObjectPtr->Tags.IsEmpty())
	{
		return;
	}

	int32 Position = ObjectPtr->GetPosition();
	if (Position == INDEX_NONE)
	{
		return;
	}

	if (PathActors.Contains(Position))
	{
	}

	PathActors.Emplace(Position, ObjectPtr);
}

AGameplayPathActor* AStageLevel::GetPathActor(int32 Position) const
{
	if (!PathActors.Contains(Position))
	{
		return nullptr;
	}

	auto& Element = PathActors[Position];
	if (!Element.IsValid())
	{
		return nullptr;
	}

	return Element.Get();
}

TArray<AGameplayPathActor*> AStageLevel::GetPathActors() const
{
	TArray<AGameplayPathActor*> Result;

	for (auto& [_, Element] : PathActors)
	{
		if (Element.IsValid())
		{
			Result.Emplace(Element.Get());
		}
	}

	return Result;
}

void AStageLevel::AddSpawner(AStageSpawner* Spawner)
{
	TWeakObjectPtr<AStageSpawner> ObjectPtr = Spawner;
	if (!ObjectPtr.IsValid())
	{
		return;
	}
	if (ObjectPtr->Tags.IsEmpty())
	{
		return;
	}
	int32 Position = ObjectPtr->GetPosition();
	if (Position == INDEX_NONE)
	{
		return;
	}
	Spawners.Emplace(Position, ObjectPtr);
}

AStageSpawner* AStageLevel::GetSpawner(int32 Position) const
{
	if (!Spawners.Contains(Position))
	{
		return nullptr;
	}

	auto& Element = Spawners[Position];

	if (!Element.IsValid())
	{
		return nullptr;
	}

	return Element.Get();
}

TArray<AStageSpawner*> AStageLevel::GetSpawners() const
{
	TArray<AStageSpawner*> Result;

	for (auto& [_, Element] : Spawners)
	{
		if (Element.IsValid())
		{
			Result.Emplace(Element.Get());
		}
	}

	return Result;
}

void AStageLevel::AddStartPoint(AStageStartPoint* StartPoint)
{
	TWeakObjectPtr<AStageStartPoint> ObjectPtr = StartPoint;
	if (!ObjectPtr.IsValid())
	{
		return;
	}
	if (ObjectPtr->Tags.IsEmpty())
	{
		return;
	}
	int32 Position = ObjectPtr->GetPosition();
	if (Position == INDEX_NONE)
	{
		return;
	}

	ObjectPtr->Setup(Supervisor);
	StartPoints.Emplace(Position, ObjectPtr);
}

AStageStartPoint* AStageLevel::GetStartPoint(int32 Position) const
{
	if (!StartPoints.Contains(Position))
	{
		return nullptr;
	}

	auto& Element = StartPoints[Position];
	if (!Element.IsValid())
	{
		return nullptr;
	}
	return Element.Get();
}

TArray<AStageStartPoint*> AStageLevel::GetStartPoints() const
{
	TArray<AStageStartPoint*> Result;

	for (auto& [_, Element] : StartPoints)
	{
		if (Element.IsValid())
		{
			Result.Emplace(Element.Get());
		}
	}
	return Result;
}

void AStageLevel::OnInitailize()
{
	for (auto& Actor : GetLevelActors())
	{
		if (Actor->IsA<AGameplayPathActor>())
		{
			AddPathActor(Cast<AGameplayPathActor>(Actor));
		}
		else if (Actor->IsA<AStageBuildZone>())
		{
			AddBuildZone(Cast<AStageBuildZone>(Actor));
		}
		else if (Actor->IsA<AStageSpawner>())
		{
			AddSpawner(Cast<AStageSpawner>(Actor));
		}
		else if (Actor->IsA<AStageStartPoint>())
		{
			AddStartPoint(Cast<AStageStartPoint>(Actor));
		}
		else if (Actor->IsA<AStagePlayerPawn>())
		{
			SetPlayerPawn(Cast<AStagePlayerPawn>(Actor));
		}
	}


	TArray<AActor*> FindActors;
}
