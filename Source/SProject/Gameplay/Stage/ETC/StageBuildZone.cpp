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
#include "Types/UnitTypes.h"
#include "Table/TowerTable.h"
#include "Gameplay/GameplayLogging.h"
#include "Gameplay/Team/GameplayTeamSubsystem.h"
#include "Gameplay/GameplayPlayer.h"
#include "Gameplay/Stage/Stage.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageUnitEvent.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Interface/IStageTower.h"
#include "Gameplay/Stage/Component/StageSpawnComponent.h"
#include "Gameplay/Stage/Component/StagePlayerComponent.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"
#include "Gameplay/Stage/AI/StageAIController.h"
#include "Gameplay/Stage/ETC/StageSupervisor.h"

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

void AStageBuildZone::Reset()
{
}

void AStageBuildZone::Load(FTower LoadedTowerData)
{
}

void AStageBuildZone::Setup(AStageSupervisor* InSupervisor)
{
	Supervisor = InSupervisor;
}

FGErrorInfo AStageBuildZone::GetTowerReceipt(FStageTowerReceipt& Receipt)
{
	Receipt = FStageTowerReceipt{};

	// NOTE. 타워가 이미 스폰되어 있을 경우
	if (SpawnedTower.IsValid())
	{
		auto SellPrice = SpawnedTower->GetSellPrice();
		if (!SellPrice.IsSet())
		{
			return GameCore::Throw(GameErr::VALUE_INVALID, TEXT("AStageBuildZone::GetTowerReceipt(Receipt):SellPrice를 찾지 못하였습니다."));
		}

		auto TowerInfo = SpawnedTower->GetTowerInfo();
		if (!TowerInfo.IsSet())
		{
			return GameCore::Throw(GameErr::VALUE_INVALID, TEXT("AStageBuildZone::GetTowerReceipt(Receipt):TowerInfo를 찾지 못하였습니다."));
		}

		int32 TowerMaxLevel = 0;
		auto Err = UStageTableHelper::GetStageTowerMaxLevel(TowerInfo->TowerType, TowerInfo->Kind, TowerMaxLevel);
		if (!GameCore::IsOK(Err))
		{
			return Err;
		}

		Receipt.bSellable = true;
		Receipt.SellPrice = *SellPrice;
		Receipt.bMaxLevel = TowerInfo->Level >= TowerMaxLevel;
		if (!Receipt.bMaxLevel)
		{
			FStageTowerInfo NextTower;
			UStageTableHelper::GetNextStageTower(TowerInfo->TowerType, TowerInfo->Kind, TowerInfo->Level, NextTower);
			Receipt.BuildTowers.Emplace(NextTower);
		}
	}
	// NOTE. 타워가 없어 건설을 해야 하는 경우
	else
	{
		if (!IsValid(BuildZoneData))
		{
			return GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("AStageBuildZone::GetTowerReceipt(Receipt):BuildZoneData가 유효하지 않습니다."));
		}

		for (auto& Content : BuildZoneData->BuildContents)
		{
			FStageTowerInfo Tower;
			if (auto Err = UStageTableHelper::GetBuildStageTower(Content.TowerType, Content.Kind, Content.Level, Tower); !GameCore::IsOK(Err))
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
	if (!Supervisor.IsValid())
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("AStageBuildZone::RequestBuildTower(BuildTowerInfo):Supervisor를 찾지 못했습니다."));
	}

	FStageTowerReceipt Receipt;
	if (auto Err = GetTowerReceipt(Receipt); !GameCore::IsOK(Err))
	{
		return Err;
	}

	// NOTE. 건설 가능한 목록에 있는지 검증
	if (!Receipt.BuildTowers.ContainsByPredicate([&BuildTowerInfo](const FStageTowerInfo& Item) { return Item == BuildTowerInfo; }))
	{
		return GameCore::Throw(GameErr::Stage::CANNOT_BUILD_TOWER, FString::Printf(TEXT("AStageBuildZone::RequestBuildTower(BuildTowerInfo):건설 할 타워를 찾지 못하였습니다.%s"), *BuildTowerInfo.ToString()));
	}

	// NOTE. 비용 지불
	if (auto Err = Supervisor->PayUsePoint(BuildTowerInfo.UsePoint); !GameCore::IsOK(Err))
	{
		return Err;
	}

	if (SpawnedTower.IsValid())
	{
		SpawnedTower->Remove();
	}

	AStageTowerUnit* SpawnedUnit = nullptr;
	auto SpawnLocation = GetBuildLocation();
	
	// NOTE. 타워 생성
	if (auto Err = Supervisor->SpawnTower(GetTeamID(), SpawnLocation, GetActorRotation(), BuildTowerInfo, SpawnedUnit); !GameCore::IsOK(Err))
	{
		return Err;
	}
	else if (!SpawnedUnit)
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("AStageBuildZone::RequestBuildTower(BuildTowerInfo):SpawnedUnit가 유효하지 않습니다."));
	}

	SpawnedTower = SpawnedUnit;
	SpawnLocation.Z = SpawnLocation.Z + SpawnedTower->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SpawnedTower->SetActorLocation(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z));

	OnCompleteBuildTower(SpawnedUnit);
	//Stage::SendUnitEvent(this, Stage::NewUnitEvent<UStageUnitEvent_Spawn>(SpawnedUnit));

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
