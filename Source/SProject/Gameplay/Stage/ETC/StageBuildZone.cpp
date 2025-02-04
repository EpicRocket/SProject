
#include "StageBuildZone.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
// include GameCore
#include "GMessage/Subsystem/GMessageSubsystem.h"
#include "Error/GErrorManager.h"
#include "Error/GErrorTypes.h"
// include Project
#include "Gameplay/GameplayLogging.h"
#include "Gameplay/Team/GameplayTeamSubsystem.h"
#include "Gameplay/Team/GameplayPlayer.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Interface/IStageTower.h"
#include "Gameplay/Stage/Unit/UnitStageTower.h"
#include "Table/TableSubsystem.h"
#include "Table/TowerTable.h"
#include "Gameplay/Stage/StageTableRepository.h"

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

FStageTowerReceipt AStageBuildZone::GetTowerReceipt() const
{
	FStageTowerReceipt Receipt;
	if (!IsValid(SpawnedTower))
	{
		if (!BuildZoneData)
		{
			UE_LOG(LogGameplay, Warning, TEXT("Actor(%s)의 BuildZoneData를 찾을 수 없습니다."), *GetFName().ToString());
			//Receipt.Error = FGErrorInfo::CreateError(TEXT("BuildZoneData를 찾을 수 없습니다."));
			Receipt.Error.ErrType = EGErrType::Error;
			return Receipt;
		}

		for (auto& Content : BuildZoneData->BuildContents)
		{
			FBuildStageTower Tower;
			if (!UStageTableHelper::GetBuildStageTower(Content.TowerType, Content.Kind, Content.Level, Tower))
			{
				continue;
			}
			Receipt.BuildTowers.Emplace(Tower);
		}
	}
	else
	{
		Receipt.bSellable = true;
		Receipt.SellPrice = SpawnedTower->GetSellPrice();

		int32 MaxLevel = UStageTableHelper::GetStageTowerMaxLevel(SpawnedTower->GetTowerType(), SpawnedTower->GetKind());
		if (SpawnedTower->GetLevel() >= MaxLevel)
		{
			Receipt.bMaxLevel = true;
		}
		else
		{
			FBuildStageTower NextTower;
			UStageTableHelper::GetNextStageTower(SpawnedTower->GetTowerType(), SpawnedTower->GetKind(), SpawnedTower->GetLevel(), NextTower);
			Receipt.BuildTowers.Emplace(NextTower);
		}
	}

	return Receipt;
}

void AStageBuildZone::RequestBuildTower(const FBuildStageTower& BuildStageTower)
{
	auto TeamSubsystem = UWorld::GetSubsystem<UGameplayTeamSubsystem>(GetWorld());
	check(TeamSubsystem);

	auto Player = TeamSubsystem->GetPlayer(GetGenericTeamId());
	if (!Player)
	{
		return;
	}

	TSubclassOf<AUnitStageTower> TowerClass;
	int64 NeedUsePoint = TNumericLimits<int64>::Max();

	switch (BuildStageTower.TowerType)
	{
	case EStageTowerType::Normal:
	{
		auto Row = UTableHelper::GetData<FNormalTowerTableRow>(BuildStageTower.Index);
		if (!Row)
		{
			return;
		}

		TowerClass = Row->UnitPath.LoadSynchronous();
		NeedUsePoint = Row->UsePoint;
	}
	break;

	default:
		return;
	}

	if (!TowerClass)
	{
		return;
	}

	if (!GameCore::IsOK(Player->ConsumeUsePoint(NeedUsePoint)))
	{
		return;
	}

	auto SpawnedLocation = GetBuildLocation();
	auto SpawnedRotation = GetActorRotation();

	AUnitStageTower* SpawnUnit = GetWorld()->SpawnActorDeferred<AUnitStageTower>(TowerClass, FTransform(SpawnedRotation, SpawnedLocation), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	SpawnUnit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//
	SpawnUnit->SetBuildReceipt(BuildStageTower);
	//

	float HalfHeight = SpawnUnit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SpawnedLocation += FVector(0.0f, 0.0f, HalfHeight);
	SpawnUnit->FinishSpawning(FTransform(SpawnedRotation, SpawnedLocation), false, nullptr, ESpawnActorScaleMethod::MultiplyWithRoot);

	if (IsValid(SpawnedTower))
	{
		SpawnedTower->Destroy();
	}

	SpawnedTower = SpawnUnit;
}

void AStageBuildZone::RequestDemolishTower()
{
	if (!IsValid(SpawnedTower))
	{
		return;
	}

	auto TeamSubsystem = UWorld::GetSubsystem<UGameplayTeamSubsystem>(GetWorld());
	check(TeamSubsystem);

	auto Player = TeamSubsystem->GetPlayer(GetGenericTeamId());
	if (!Player)
	{
		return;
	}
	auto Receipt = GetTowerReceipt();
	if (!Receipt.bSellable)
	{
		return;
	}

	/*if (!GameCore::IsOK(Player->AddUsePoint(Receipt.SellPrice)))
	{
		return;
	}*/

	SpawnedTower->Destroy();
	SpawnedTower = nullptr;
}
