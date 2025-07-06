// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageSupervisor.h"
// include GGameCore
#include "Core/GGameCoreHelper.h"
// include Project
#include "Types/StageTypes.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/ETC/StageBuildZone.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Types/StageMonsterTypes.h"
#include "Gameplay/Stage/Component/StageStateComponent.h"
#include "Gameplay/Stage/Component/StageStorageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSupervisor)

void AStageSupervisor::BeginPlay()
{
	Super::BeginPlay();

	OwnerLevel = Cast<AStageLevel>(GetOwner());
	if (!OwnerLevel.IsValid())
	{
		GameCore::Throw(GameErr::WORLD_INVALID, TEXT("AStageSupervisor::BeginPlay() OwnerLevel를 찾을 수 없습니다."));
		return;
	}

	StageStateComponent = UGGameCoreHelper::GetGameStateComponent<UStageStateComponent>(this);
	if (!StageStateComponent.IsValid())
	{
		GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("AStageSupervisor::BeginPlay() StageStateComponent를 찾을 수 없습니다."));
		return;
	}

	StageStorageComponent = UGGameCoreHelper::GetPlayerControllerComponent<UStageStorageComponent>(StageStateComponent->PrimaryPC.Get());
	if (!StageStorageComponent.IsValid())
	{
		GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("AStageSupervisor::BeginPlay() StageStorageComponent를 찾을 수 없습니다."));
		return;
	}

	StageTableReceipt = NewObject<UStageTableReceipt>(this, NAME_None, RF_Public | RF_Transient);

	auto StageTableRepo = UStageTableRepository::Get(this);
	if (!StageTableRepo)
	{
		return;
	}

	// NOTE. 스테이지에서 사용 할 로드 리스트 생성
	TMap<EStageTowerType, TSet<int32>> LoadTowerList;
	for (auto BuildZone : OwnerLevel->GetBuildZones())
	{
		TObjectPtr<UStageBuildZoneData> BuildZoneAsset = BuildZone->BuildZoneData;
		if (!IsValid(BuildZoneAsset))
		{
			continue;
		}

		for (auto& Content : BuildZoneAsset->BuildContents)
		{
			LoadTowerList.FindOrAdd(Content.TowerType).Emplace(Content.Kind);
		}
	}

	// NOTE. 테이블 로드 요청
	StageTableRepo->Load(StageTableReceipt, OwnerLevel->StageLevel, LoadTowerList).Then(
		[this, ThisPtr = TWeakObjectPtr<AStageSupervisor>(this)](TFuture<FGErrorInfo> Error)
		{
			if (!ThisPtr.IsValid())
			{
				return;
			}

			if (!GameCore::IsOK(Error.Get()))
			{
				// TODO: 여기 들어온다면 스테이지 자체를 못 여는 거임...
				return;
			}

			OnTableLoaded();
		}
	);
}

void AStageSupervisor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AStageSupervisor::SetHp(int32 NewValue)
{
	if (Stage.IsValid())
	{
		return;
	}
	int32 OldValue = Stage.Pin()->Hp;
	Stage.Pin()->Hp = NewValue;
	OnHpChanged.Broadcast(OldValue, NewValue);
}

int32 AStageSupervisor::GetHp() const
{
	if (!Stage.IsValid())
	{
		return 0;
	}
	return Stage.Pin()->Hp;
}

void AStageSupervisor::SetUsePoint(int32 NewValue)
{
	if (!Stage.IsValid())
	{
		return;
	}
	int32 OldValue = Stage.Pin()->UsePoint;
	Stage.Pin()->UsePoint = NewValue;
	OnUsePointChanged.Broadcast(OldValue, NewValue);
}

int32 AStageSupervisor::GetUsePoint() const
{
	if (!Stage.IsValid())
	{
		return 0;
	}
	return Stage.Pin()->UsePoint;
}

void AStageSupervisor::OnTableLoaded()
{
	if (!StageStateComponent.IsValid())
	{
		return;
	}

	if (!IsValid(StageTableReceipt))
	{
		return;
	}

	for (auto Context : StageTableReceipt->Contexts)
	{
		Context->Load();
	}

	StageStateComponent->AddStageLoadFlags(EStageLoadFlags::Repository, GameCore::Pass());

	OnGameplayDataLoad();
}

void AStageSupervisor::OnGameplayDataLoad()
{
	if (!OwnerLevel.IsValid())
	{
		return;
	}

	if (!StageStateComponent.IsValid())
	{
		return;
	}

	if (!StageStorageComponent.IsValid())
	{
		return;
	}

	Stage = StageStorageComponent->GetStage(OwnerLevel->StageLevel);
	if (!Stage.IsValid())
	{
		return;
	}

	for (auto& TowerData : Stage.Pin()->Towers)
	{
		auto SelectedBuildZone = OwnerLevel->GetBuildZone(TowerData.Position);
		if (!IsValid(SelectedBuildZone))
		{
			continue;
		}

		SelectedBuildZone->Load(TowerData);
	}

	StageStateComponent->AddStageLoadFlags(EStageLoadFlags::GameplayData, GameCore::Pass());
}
