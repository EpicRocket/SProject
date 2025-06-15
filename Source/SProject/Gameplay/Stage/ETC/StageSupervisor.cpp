// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageSupervisor.h"
// include Engine
#include "Components/GameFrameworkComponentManager.h"
// include Project
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/ETC/StageBuildZone.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"

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
	/*StageTableRepo->Load(OwnerLevel->StageLevel, LoadTowerList).Then(
		[this, ThisPtr = TWeakObjectPtr<AStageSupervisor>(this)](TFuture<UStageTableReceipt*> Receipt)
		{
			if (!ThisPtr.IsValid())
			{
				return;
			}

			OnTableLoaded(Receipt.Get());
		}
	);*/
}

void AStageSupervisor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	// NOTE. 테이블 언로드
	/*if (auto Repo = UStageTableRepository::Get(this))
	{
		Repo->Unload(StageTableReceipt);
	}*/

	Super::EndPlay(EndPlayReason);
}

//void AStageSupervisor::OnTableLoaded(UStageTableReceipt* Receipt)
//{
//	StageTableReceipt = Receipt;
//}
