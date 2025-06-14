// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageBuildZone.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
// include GameCore
#include "GMessage/Subsystem/GMessageSubsystem.h"
#include "Error/GError.h"
// include Project
#include "Table/TowerTable.h"
#include "Gameplay/GameplayLogging.h"
#include "Gameplay/Team/GameplayTeamSubsystem.h"
#include "Gameplay/GameplayPlayer.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageUnitEvent.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Interface/IStageTower.h"
#include "Gameplay/Stage/Component/StageSpawnComponent.h"
#include "Gameplay/Stage/Component/StagePlayerComponent.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"
#include "Gameplay/Stage/AI/StageAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageBuildZone)

namespace Stage 
{
	UE_DEFINE_GAMEPLAY_TAG(Selected_BuildZone, "Gameplay.Stage.Selected.BuildZone");
}

FName AStageBuildZone::InteractionComponentName = TEXT("InteractionComponent");

AStageBuildZone::AStageBuildZone()
{
	InteractionComponent = CreateDefaultSubobject<UBoxComponent>(InteractionComponentName);
	InteractionComponent->SetBoxExtent(FVector(100.0f, 100.0f, 150.0f));
	static FName InteractionCollsionProfileName = FName(TEXT("UI"));
	InteractionComponent->SetCollisionProfileName(InteractionCollsionProfileName);
	RootComponent = InteractionComponent;
}

FGErrorInfo AStageBuildZone::K2_GetTowerReceipt(FStageTowerReceipt& Receipt)
{
	// NOTE. 타워가 이미 스폰되어 있을 경우
	if (SpawnedTower.IsValid())
	{
		auto SellPrice = SpawnedTower->GetSellPrice();
		if (!SellPrice.IsSet())
		{
			return GameCore::Throw(GameErr::VALUE_INVALID, TEXT("SpawnedTower->GetSellPrice()"));
		}

		auto TowerInfo = SpawnedTower->GetTowerInfo();
		if (!TowerInfo.IsSet())
		{
			return GameCore::Throw(GameErr::VALUE_INVALID, TEXT("SpawnedTower->GetTowerInfo()"));
		}

		int32 TowerMaxLevel = 0;
		auto Err = UStageTableHelper::GetStageTowerMaxLevel(this, TowerInfo->TowerType, TowerInfo->Kind, TowerMaxLevel);
		if (!GameCore::IsOK(Err))
		{
			return Err;
		}

		Receipt = FStageTowerReceipt{};
		Receipt.bSellable = true;
		Receipt.SellPrice = *SellPrice;
		Receipt.bMaxLevel = TowerInfo->Level >= TowerMaxLevel;
		if (Receipt.bMaxLevel)
		{
			FStageTowerInfo NextTower;
			UStageTableHelper::GetNextStageTower(this, TowerInfo->TowerType, TowerInfo->Kind, TowerInfo->Level, NextTower);
			Receipt.BuildTowers.Emplace(NextTower);
		}
	}
	// NOTE. 타워가 없어 건설을 해야 하는 경우
	else
	{
		if (!IsValid(BuildZoneData))
		{
			return GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("BuildZoneData"));
		}

		Receipt = FStageTowerReceipt{};
		for (auto& Content : BuildZoneData->BuildContents)
		{
			FStageTowerInfo Tower;
			if (auto Err = UStageTableHelper::GetBuildStageTower(this, Content.TowerType, Content.Kind, Content.Level, Tower); !GameCore::IsOK(Err))
			{
				continue;
			}
			Receipt.BuildTowers.Emplace(Tower);
		}
	}

	return GameCore::Pass();
}

FGErrorInfo AStageBuildZone::RequestBuildTower(const FStageTowerInfo& BuildTowerInfo)
{
	AddUsePoint(-BuildTowerInfo.UsePoint);

	if (SpawnedTower.IsValid())
	{
		SpawnedTower->Remove();
	}

	AStageTowerUnit* SpawnedUnit = nullptr;
	auto SpawnLocation = GetBuildLocation();

	auto Err = UStageSpawnHelper::SpawnTower(
		GetTeamID(),
		SourceStage.Get(),
		SpawnLocation,
		GetActorRotation(),
		BuildTowerInfo,
		nullptr,
		SpawnedUnit
	);

	if (!GameCore::IsOK(Err))
	{
		return Err;
	}

	SpawnedTower = SpawnedUnit;
	SpawnLocation.Z = SpawnLocation.Z + SpawnedTower->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SpawnedTower->SetActorLocation(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z));

	if (auto AIController = SpawnedUnit->GetController<AStageAIController>())
	{
		AIController->SetGenericTeamId(GetTeamID());
		AIController->SourceStage = SourceStage;
		AIController->AIBehaviorTree = BuildTowerInfo.AI;
	}

	OnCompleteBuildTower(SpawnedUnit);
	Stage::SendUnitEvent(this, Stage::NewUnitEvent<UStageUnitEvent_Spawn>(SpawnedUnit));

	return GameCore::Pass();
}

FGErrorInfo AStageBuildZone::RequestDemolishTower(const int64 SellPrice)
{
	if (!SpawnedTower.IsValid())
	{
		return GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("SpawnedTower"));
	}

	AddUsePoint(SellPrice);

	OnCompleteDemolishTower(SpawnedTower.Get());
	SpawnedTower->Kill();
	SpawnedTower = nullptr;

	return GameCore::Pass();
}

void AStageBuildZone::AddUsePoint(int64 Point)
{
	auto TeamSubsystem = UWorld::GetSubsystem<UGameplayTeamSubsystem>(GetWorld());
	if (!TeamSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UGameplayTeamSubsystem"));
		return;
	}

	auto Player = TeamSubsystem->GetPlayer(GetTeamID());
	if (!Player)
	{
		GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("Player"));
		return;
	}

	auto StagePlayerCom = Player->GetComponentByClass<UStagePlayerComponent>();
	if (!IsValid(StagePlayerCom))
	{
		GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("UStagePlayerComponent"));
		return;
	}

	StagePlayerCom->AddUsePoint(Point);
}
