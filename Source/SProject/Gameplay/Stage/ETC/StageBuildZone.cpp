
#include "StageBuildZone.h"
// include Engine
#include "Engine/GameInstance.h"
#include "Components/BoxComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/CapsuleComponent.h"
// include GameCore
#include "GMessage/Subsystem/GMessageSubsystem.h"
// include Project
#include "Gameplay/GameplayLogging.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Interface/IStageTower.h"
#include "Gameplay/Stage/Unit/UnitStageTower.h"
#include "Table/TableSubsystem.h"
#include "Table/TowerTable.h"
#include "TableRepository/StageTableRepository.h"
#include "TableTypes/StageTableTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageBuildZone)

namespace Stage
{
	UE_DEFINE_GAMEPLAY_TAG(Selected_BuildZone, "Gameplay.Stage.Selected.BuildZone");
}

FName AStageBuildZone::InteractionComponentName = TEXT("InteractionComponent");
FName AStageBuildZone::ChildActorComponentName = TEXT("ChildActorComponent");

AStageBuildZone::AStageBuildZone()
{
	InteractionComponent = CreateDefaultSubobject<UBoxComponent>(InteractionComponentName);
	InteractionComponent->SetBoxExtent(FVector(100.0f, 100.0f, 150.0f));
	static FName InteractionCollsionProfileName = FName(TEXT("UI"));
	InteractionComponent->SetCollisionProfileName(InteractionCollsionProfileName);
	RootComponent = InteractionComponent;

	ChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(ChildActorComponentName);
	ChildActorComponent->SetupAttachment(InteractionComponent);
}

FStageTowerReceipt AStageBuildZone::GetTowerReceipt() const
{
	FStageTowerReceipt Receipt;

	if (!SpawnedTower)
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


// TODO: 별도의 AI 컨트롤 필요함...
#include "AIController.h"
#include "Engine/World.h"

void AStageBuildZone::RequestBuildTower(const FBuildStageTower& BuildStageTower)
{
	// TODO: 플레이어 자원 체크

	TSubclassOf<AUnitStageTower> TowerClass;

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
	}
	break;

	default:
		return;
	}

	if (!TowerClass)
	{
		return;
	}

	ChildActorComponent->SetChildActorClass(TowerClass);
	auto TowerActor = Cast<AUnitStageTower>(ChildActorComponent->GetChildActor());
	if (!TowerActor)
	{
		return;
	}

	TowerActor->AIControllerClass = AAIController::StaticClass();
	TowerActor->SpawnDefaultController();
	
	// TODO: 임시임...
	auto CapsuleComponent = TowerActor->GetCapsuleComponent();
	auto TargetLocation = GetBuildLocation() + FVector(0.0F, 0.0F, CapsuleComponent->GetScaledCapsuleHalfHeight());
	TowerActor->SetActorLocation(TargetLocation);
}
