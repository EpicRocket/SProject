// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageSupervisor.h"
// include GGameCore
#include "Core/GGameCoreHelper.h"
#include "Table/GTableHelper.h"
// include Project
#include "Types/StageTypes.h"
#include "Table/ConstTable.h"
#include "Table/StageTable.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/Stage.h"
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

FGErrorInfo AStageSupervisor::SpawnTower(uint8 TeamID, FVector Location, FRotator Rotation, FStageTowerInfo TowerInfo, AStageTowerUnit*& SpawnedUnit)
{
	return FGErrorInfo();
}

FGErrorInfo AStageSupervisor::SpawnMonster(uint8 TeamID, FVector Location, FRotator Rotation, FStageMonsterInfo MonsterInfo, AStageMonsterUnit*& SpawnedUnit)
{
	return FGErrorInfo();
}

//FGErrorInfo AStageSupervisor::K2_SpawnUnit(uint8 TeamID, FVector Location, FRotator Rotation, TSubclassOf<AStageUnitCharacter> StageUnitClass, AStageUnitCharacter*& SpawnedUnit)
//{
//	return FGErrorInfo();
//}

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

FGErrorInfo AStageSupervisor::PayUsePoint(int32 Cost)
{
	// TODO: 재화 소모
	return GameCore::Pass();
}

void AStageSupervisor::StartStage()
{
	if (!Stage.IsValid())
	{
		// TODO: 에러... 스테이지에 대한 정보를 찾지 못한거라서 로그인 페이지로 돌아가게 하던가 돌아갈 수 있는 스테이지가 있으면 거기로 이동 시켜줘야 할듯함...
		return;
	}

	auto StageRef = Stage.Pin().ToSharedRef();
	auto Flags = static_cast<EStageFlags>(StageRef->Flags);

	// NOTE. 상태가 없다면 신규 상태로 만든다.
	if (Flags == EStageFlags::None)
	{
		EnumAddFlags(Flags, EStageFlags::New);
	}

	bool bComplated = EnumHasAnyFlags(Flags, EStageFlags::Completed);
	auto Result = [&Flags, bComplated](EStageFlags Target)
		{
			Flags = Target;
			if (bComplated)
			{
				EnumAddFlags(Flags, EStageFlags::Completed);
			}
		};

	if (EnumHasAnyFlags(Flags, EStageFlags::New))
	{
		Result(EStageFlags::New);
		StageRef->Flags = static_cast<uint8>(Flags);
		OnNewStage();
	}
	else if (EnumHasAnyFlags(Flags, EStageFlags::Defeat))
	{
		Result(EStageFlags::Defeat);
		StageRef->Flags = static_cast<uint8>(Flags);
		OnDefeatStage();
	}
	else if (EnumHasAnyFlags(Flags, EStageFlags::Clear))
	{
		Result(EStageFlags::Clear);
		StageRef->Flags = static_cast<uint8>(Flags);
		OnClearStage();
	}
	else if (bComplated)
	{
		// NOTE. 완료 한 스테이지 이지만 선택 된 경우
		StageRef->Flags = static_cast<uint8>(EStageFlags::Completed);
		OnCompletedStage();
	}
	else
	{
		Result(EStageFlags::Playing);
		StageRef->Flags = static_cast<uint8>(Flags);
		OnPlayingStage();
	}
}

void AStageSupervisor::ResetStageData()
{
	if (!Stage.IsValid())
	{
		return;
	}

	if (!StageStorageComponent.IsValid())
	{
		return;
	}

	auto StageRef = Stage.Pin().ToSharedRef();
	auto StageTableRow = UGTableHelper::GetTableData<FStageTableRow>(StageRef->Level);
	if (!StageTableRow)
	{
		return;
	}

	// NOTE. New 플래그 제거
	auto Flags = static_cast<EStageFlags>(StageRef->Flags);
	EnumRemoveFlags(Flags, EStageFlags::New);
	StageRef->Flags = static_cast<uint8>(Flags);

	StageRef->Wave = 0;
	StageRef->Towers.Empty();
	StageRef->Hp = GetDefault<UConstSettings>()->UserHp;
	StageRef->UsePoint = StageTableRow->UsePoint;

	StageStorageComponent->SetStage(*StageRef);
	OnGameplayDataReload();
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

	for (auto BuildZone : OwnerLevel->GetBuildZones())
	{
		BuildZone->Reset();
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

void AStageSupervisor::OnGameplayDataReload()
{
	if (!OwnerLevel.IsValid())
	{
		return;
	}

	if (!StageStateComponent.IsValid())
	{
		return;
	}

	for (auto BuildZone : OwnerLevel->GetBuildZones())
	{
		BuildZone->Reset();
	}
}
