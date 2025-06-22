// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageSupervisor.h"
// include Engine
#include "Components/GameFrameworkComponentManager.h"
// include GGameCore
#include "Core/GGameCoreHelper.h"
// include Project
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/ETC/StageBuildZone.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Types/StageMonsterTypes.h"
#include "Gameplay/Stage/Component/StageStateComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSupervisor)

void AStageSupervisor::BeginPlay()
{
	Super::BeginPlay();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

	if (!OwnerLevel.IsValid())
	{
		UE_LOG(LogStage, Error, TEXT("OwnerLevel를 찾을 수 없습니다."));
		return;
	}

	auto StageTableRepo = UStageTableRepository::Get(this);
	if (!StageTableRepo)
	{
		return;
	}
	
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
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AStageSupervisor::OnTableLoaded()
{
	auto StageStateComp = UGGameCoreBlueprintFunctionLibrary::GetGameStateComponent<UStageStateComponent>(this);
	if (!StageStateComp)
	{
		GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("StageStateComponent"));
		return;
	}

	if (!IsValid(StageTableReceipt))
	{
		GameCore::Throw(GameErr::POINTER_INVALID, TEXT("StageTableReceipt"));
		return;
	}

	for (auto Context : StageTableReceipt->Contexts)
	{
		Context->Load();
	}

	StageStateComp->AddStageLoadFlags(EStageLoadFlags::Repository, GameCore::Pass());
}
