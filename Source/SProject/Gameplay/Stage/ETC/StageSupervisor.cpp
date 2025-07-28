// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageSupervisor.h"
// include Engine
#include "Engine/World.h"
// include GGameCore
#include "Core/GGameCoreHelper.h"
#include "Table/GTableHelper.h"
// include Project
#include "Types/StageTypes.h"
#include "Table/ConstTable.h"
#include "Table/StageTable.h"
#include "Gameplay/GameplayHelper.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/Stage.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/ETC/StageBuildZone.h"
#include "Gameplay/Stage/ETC/StageSpawner.h"
#include "Gameplay/Stage/Types/GameplayStageTypes.h"
#include "Gameplay/Stage/Component/StageStateComponent.h"
#include "Gameplay/Stage/Component/StageStorageComponent.h"
#include "Gameplay/Stage/Component/StageSpawnComponent.h"
#include "Gameplay/Stage/Component/StageWaveComponent.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"
#include "Gameplay/Stage/Unit/StageMonsterUnit.h"
#include "Gameplay/Stage/AI/StageAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSupervisor)

void AStageSupervisor::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	auto SpawnComponent = GetSpawnComponent();
	check(SpawnComponent);

	auto WaveComponent = GetWaveComponent();
	check(WaveComponent);

	WaveComponent->RequestStageSpawnEvent.BindDynamic(this, &AStageSupervisor::OnWaveMonsterSpawn);
}

void AStageSupervisor::Setup(AStageLevel* OwnerStage)
{
	OwnerLevel = OwnerStage;
	if (!OwnerLevel.IsValid())
	{
		GameCore::Throw(GameErr::WORLD_INVALID, TEXT("BeginPlay() OwnerLevel를 찾을 수 없습니다."));
		return;
	}

	StageStateComponent = UGGameCoreHelper::GetGameStateComponent<UStageStateComponent>(this);
	if (!StageStateComponent.IsValid())
	{
		GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("BeginPlay() StageStateComponent를 찾을 수 없습니다."));
		return;
	}

	StageStorageComponent = UGGameCoreHelper::GetPlayerControllerComponent<UStageStorageComponent>(StageStateComponent->PrimaryPC.Get());
	if (!StageStorageComponent.IsValid())
	{
		GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("BeginPlay() StageStorageComponent를 찾을 수 없습니다."));
		return;
	}

	int32 StageLevel = OwnerLevel->StageLevel;

	StagePtr = StageStorageComponent->GetStage(StageLevel);
	StageTableReceipt = NewObject<UStageTableReceipt>(this, NAME_None, RF_Public | RF_Transient);

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
	UStageTableRepository::Get().Load(StageTableReceipt, StageLevel, LoadTowerList).Then(
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

			if (auto Err = OnTableLoaded(); !GameCore::IsOK(Err))
			{
				return;
			}
		}
	);
}

FGErrorInfo AStageSupervisor::K2_GetStage(UPARAM(DisplayName = "ReturnValue") FStage& CurrentStage) const
{
	if (!StagePtr.IsValid())
	{
		CurrentStage = FStage{};
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("K2_GetStage():Stage를 찾을 수 없습니다."));
	}
	CurrentStage = *StagePtr.Pin();
	return GameCore::Pass();
}

TSharedPtr<FStage> AStageSupervisor::GetStage() const
{
	if (!StagePtr.IsValid())
	{
		return nullptr;
	}
	return StagePtr.Pin();
}

FGErrorInfo AStageSupervisor::RegisterSpawnedUnit(AStageUnitCharacter* Unit)
{
	if (!IsValid(Unit))
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("RegisterSpawnedUnit(Unit):Unit is invalid."));
	}

	SpawnedUnits.Emplace(Unit->GetActorGuid().ToString(EGuidFormats::Digits), Unit);
	return GameCore::Pass();
}

FGErrorInfo AStageSupervisor::UnregisterSpawnedUnit(AStageUnitCharacter* Unit)
{
	if (!IsValid(Unit))
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("RegisterSpawnedUnit(Unit):Unit is invalid."));
	}

	SpawnedUnits.Remove(Unit->GetActorGuid().ToString(EGuidFormats::Digits));
	return GameCore::Pass();
}

FGErrorInfo AStageSupervisor::SpawnTower(uint8 InTeamID, FVector InLocation, FRotator InRotation, FStageTowerInfo InTowerInfo, AStageTowerUnit*& SpawnedTower)
{
	auto SpawnComponent = GetSpawnComponent();

	AStageUnitCharacter* SpawnedUnit = nullptr;
	if (auto Err = SpawnComponent->BeginSpawn(InTowerInfo.UnitClass, InLocation, InRotation, AIControllerClass, SpawnedUnit); !GameCore::IsOK(Err))
	{
		return Err;
	}

	SpawnedTower = Cast<AStageTowerUnit>(SpawnedUnit);
	if (!SpawnedTower)
	{
		auto ErrStr = FString::Printf(TEXT("SpawnTower(InTeamID:%d, InLocation:%s, InRotation:%s, InTowerInfo:%s, SpawnedTower):SpawnedTower fail cast.")
			, InTeamID, *InLocation.ToString(), *InRotation.ToString(), *InTowerInfo.ToString());
		return GameCore::Throw(GameErr::ACTOR_INVALID, ErrStr);
	}

	// 타워 정보 설정
	SpawnedTower->Setup(InTowerInfo);

	// 스폰 성공
	if (auto Err = SpawnComponent->EndSpawn(SpawnedUnit, InLocation, InRotation); !GameCore::IsOK(Err))
	{
		return Err;
	}

	// AI 설정
	if (auto AIController = Cast<AStageAIController>(SpawnedUnit->GetController()))
	{
		AIController->Setup(this, InTeamID, InTowerInfo.AI);
	}

	// 유닛 등록
	if (auto Err = RegisterSpawnedUnit(SpawnedTower); !GameCore::IsOK(Err))
	{
		return Err;
	}

	return GameCore::Pass();
}

FGErrorInfo AStageSupervisor::SpawnMonster(uint8 InTeamID, FVector InLocation, FRotator InRotation, FStageMonsterInfo InMonsterInfo, AStageMonsterUnit*& SpawnedMonster)
{
	auto SpawnComponent = GetSpawnComponent();

	AStageUnitCharacter* SpawnedUnit = nullptr;
	if (auto Err = SpawnComponent->BeginSpawn(InMonsterInfo.UnitClass, InLocation, InRotation, AIControllerClass, SpawnedUnit); !GameCore::IsOK(Err))
	{
		return Err;
	}

	SpawnedMonster = Cast<AStageMonsterUnit>(SpawnedUnit);
	if (!SpawnedMonster)
	{
		auto ErrStr = FString::Printf(TEXT("SpawnTower(InTeamID:%d, InLocation:%s, InRotation:%s, InMonsterInfo:%s, SpawnedTower):SpawnedTower fail cast.")
			, InTeamID, *InLocation.ToString(), *InRotation.ToString(), *InMonsterInfo.ToString());
		return GameCore::Throw(GameErr::ACTOR_INVALID, ErrStr);
	}

	// 몬스터 정보 설정
	SpawnedMonster->Setup(InMonsterInfo);

	// 스폰 성공
	if (auto Err = SpawnComponent->EndSpawn(SpawnedUnit, InLocation, InRotation); !GameCore::IsOK(Err))
	{
		return Err;
	}

	// AI 설정
	if (auto AIController = Cast<AStageAIController>(SpawnedUnit->GetController()))
	{
		AIController->Setup(this, InTeamID, InMonsterInfo.AI);
	}

	// 유닛 등록
	if (auto Err = RegisterSpawnedUnit(SpawnedMonster); !GameCore::IsOK(Err))
	{
		return Err;
	}

	return GameCore::Pass();
}

FGErrorInfo AStageSupervisor::RequestStartWave()
{
	auto WaveComponent = GetWaveComponent();

	if (WaveComponent->IsNextWaveLocked())
	{
		return GameCore::Throw(GameErr::Stage::CANNOT_START_NEXT_WAVE);
	}

	if (WaveComponent->IsComplete())
	{
		return GameCore::Throw(GameErr::Stage::LAST_WAVE);
	}

	return WaveComponent->Start();
}

void AStageSupervisor::SetHp(int32 NewValue)
{
	auto Stage = GetStage();
	if (Stage.IsValid())
	{
		return;
	}
	int32 OldValue = Stage->Hp;
	Stage->Hp = NewValue;
	OnHpChanged.Broadcast(OldValue, NewValue);
}

int32 AStageSupervisor::GetHp() const
{
	auto Stage = GetStage();
	if (!Stage.IsValid())
	{
		return 0;
	}
	return Stage->Hp;
}

void AStageSupervisor::SetUsePoint(int32 NewValue)
{
	auto Stage = GetStage();
	if (!Stage.IsValid())
	{
		return;
	}
	int32 OldValue = Stage->UsePoint;
	Stage->UsePoint = NewValue;
	OnUsePointChanged.Broadcast(OldValue, NewValue);
}

int32 AStageSupervisor::GetUsePoint() const
{
	auto Stage = GetStage();
	if (!Stage.IsValid())
	{
		return 0;
	}
	return Stage->UsePoint;
}

FGErrorInfo AStageSupervisor::PayUsePoint(int32 Cost)
{
	// TODO: 재화 소모
	return GameCore::Pass();
}

FGErrorInfo AStageSupervisor::StartStage()
{
	auto Stage = GetStage();
	if (!Stage.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("StartStage():Stage를 찾을 수 없습니다."));
	}

	auto Flags = static_cast<EStageFlags>(Stage->Flags);

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
		Stage->Flags = static_cast<uint8>(Flags);
		OnNewStage();
	}
	else if (EnumHasAnyFlags(Flags, EStageFlags::Defeat))
	{
		Result(EStageFlags::Defeat);
		Stage->Flags = static_cast<uint8>(Flags);
		OnDefeatStage();
	}
	else if (EnumHasAnyFlags(Flags, EStageFlags::Clear))
	{
		Result(EStageFlags::Clear);
		Stage->Flags = static_cast<uint8>(Flags);
		OnClearStage();
	}
	else if (bComplated)
	{
		// NOTE. 완료 한 스테이지 이지만 선택 된 경우
		Stage->Flags = static_cast<uint8>(EStageFlags::Completed);
		OnCompletedStage();
	}
	else
	{
		Result(EStageFlags::Playing);
		Stage->Flags = static_cast<uint8>(Flags);
		OnPlayingStage();
	}

	return GameCore::Pass();
}

FGErrorInfo AStageSupervisor::ResetStageData()
{
	auto Stage = GetStage();
	if (!Stage.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("ResetStageData():Stage를 찾을 수 없습니다."));
	}

	auto StageTableRow = UGTableHelper::GetTableData<FStageTableRow>(Stage->Level);
	if (!StageTableRow)
	{
		FString Msg = FString::Printf(TEXT("ResetStageData():StageTableRow를 찾을 수 없습니다. [Key:%d]"), Stage->Level);
		return GameCore::Throw(GameErr::TABLE_INVALID, Msg);
	}

	// NOTE. New 플래그 제거
	auto Flags = static_cast<EStageFlags>(Stage->Flags);
	EnumRemoveFlags(Flags, EStageFlags::New);
	Stage->Flags = static_cast<uint8>(Flags);

	Stage->Wave = 0;
	Stage->Towers.Empty();
	Stage->Hp = GetDefault<UConstSettings>()->UserHp;
	Stage->UsePoint = StageTableRow->UsePoint;

	if (auto Err = OnGameplayDataReload(); !GameCore::IsOK(Err))
	{
		return Err;
	}

	return GameCore::Pass();
}

void AStageSupervisor::OnWaveMonsterSpawn(FStageSpawnParam Param)
{
	if (!Param.Context.IsValid())
	{
		UE_LOGFMT(LogStage, Warning, "OnWaveMonsterSpawn(Param):UStageMonsterContext가 유효하지 않습니다.");
		return;
	}

	bool bFailure = false;

	auto Spawner = OwnerLevel->GetSpawner(Param.Spawner);
	if (!Spawner)
	{
		UE_LOGFMT(LogStage, Warning, "OnWaveMonsterSpawn(Param):Spawner를 찾지 못하였습니다.[Spawner:{0}]", Param.Spawner);
		bFailure = true;
	}

	auto Path = OwnerLevel->GetPathActor(Param.Path);
	if (!Spawner)
	{
		UE_LOGFMT(LogStage, Warning, "OnWaveMonsterSpawn(Param):Path를 찾지 못하였습니다.[Spawner:{0}]", Param.Path);
		bFailure = true;
	}

	if (bFailure)
	{
		return;
	}

	TArray<AStageMonsterUnit*> SpawnedMonsters;
	SpawnedMonsters.Reserve(Param.SpawnCount);
	for (int32 Index = 0; Index < Param.SpawnCount; Index++)
	{
		AStageMonsterUnit* SpawnedMonster;
		auto Err = SpawnMonster(Spawner->GetTeamID(), Spawner->GetSpawnLocation(), Spawner->GetSpawnRotation(), Param.Context->MonsterInfo, SpawnedMonster);
		if (!GameCore::IsOK(Err))
		{
			continue;
		}

		SpawnedMonsters.Emplace(SpawnedMonster);
	}

	for (auto Monster : SpawnedMonsters)
	{
		auto AIController = Cast<AStageAIController>(Monster->GetController());
		if (!IsValid(AIController))
		{
			continue;
		}
		UGameplayHelper::SetGameplayTagByInt32(AIController, TEXT("Path"), Param.Path);
		AIController->StartAI();
	}
}

FGErrorInfo AStageSupervisor::OnTableLoaded()
{
	if (!IsValid(StageTableReceipt))
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("ResetStageData():StageTableReceipt를 찾을 수 없습니다."));
	}

	StageTableReceipt->StageContext->Load();

	StageStateComponent->AddStageLoadFlags(EStageLoadFlags::Repository, GameCore::Pass());

	return OnGameplayDataLoad();
}

FGErrorInfo AStageSupervisor::OnGameplayDataLoad()
{
	OnGameplayDataLoad_Wave();
	OnGameplayDataLoad_User();

	StageStateComponent->AddStageLoadFlags(EStageLoadFlags::GameplayData, GameCore::Pass());

	return GameCore::Pass();
}

FGErrorInfo AStageSupervisor::OnGameplayDataLoad_Wave()
{
	auto WaveComponent = GetWaveComponent();

	auto Stage = GetStage();
	if (!Stage.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("OnGameplayDataLoad_Wave():Stage를 찾을 수 없습니다."));
	}

	if (auto Err = WaveComponent->Setup(GetStage()); !GameCore::IsOK(Err))
	{
		return Err;
	}

	return GameCore::Pass();
}

FGErrorInfo AStageSupervisor::OnGameplayDataLoad_User()
{
	auto Stage = GetStage();
	if (!Stage.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("OnGameplayDataLoad_User():Stage를 찾을 수 없습니다."));
	}

	for (auto BuildZone : OwnerLevel->GetBuildZones())
	{
		BuildZone->Reset();
	}

	for (auto& TowerData : Stage->Towers)
	{
		auto SelectedBuildZone = OwnerLevel->GetBuildZone(TowerData.Position);
		if (!IsValid(SelectedBuildZone))
		{
			continue;
		}

		SelectedBuildZone->Load(TowerData);
	}

	return GameCore::Pass();
}

FGErrorInfo AStageSupervisor::OnGameplayDataReload()
{
	for (auto BuildZone : OwnerLevel->GetBuildZones())
	{
		BuildZone->Reset();
	}

	return GameCore::Pass();
}
