// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageWaveComponent.h"
// include GameCore
//#include "Table/GTableHelper.h"
// include Project
#include "Types/StageTypes.h"
#include "Gameplay/Stage/Stage.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/GameplayStageTypes.h"

UStageWaveComponent::UStageWaveComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStageWaveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsNextWaveLocked())
	{
		NextWaveDelay -= DeltaTime;
	}

	if (IsPlaying())
	{
		TSet<int32> DeleteIds;
		DeleteIds.Reserve(SpawnDatas.Num());
		for (auto& Data : SpawnDatas)
		{
			if (Data.Amount <= 0)
			{
				DeleteIds.Emplace(Data.UniqueId);
				continue;
			}

			if (Data.SpawnDelay > 0.0)
			{
				Data.SpawnDelay -= DeltaTime;
				continue;
			}

			Data.SpawnDelay = Data.AmountDelayTime;
			int32 SpawnCount = Data.AmountValue;
			if (SpawnCount > Data.Amount)
			{
				SpawnCount = Data.Amount;
			}
			Data.Amount -= SpawnCount;

			FStageSpawnParam Param;
			Param.Context = Data.MonsterContext;
			Param.Spawner = Data.Spawner;
			Param.Path = Data.Path;
			Param.SpawnCount = SpawnCount;

			OnSpawn(Param);
		}

		if (DeleteIds.Num() > 0)
		{
			SpawnDatas.RemoveAll([&DeleteIds](const FStageSpawnData& Item)
				{
					return DeleteIds.Contains(Item.UniqueId);
				});
		}
	}
}

FGErrorInfo UStageWaveComponent::Setup(TSharedPtr<FStage> Stage)
{
	if (!Stage.IsValid())
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("Setup(Stage):Stage가 유효하지 않습니다."));
	}

	if (auto Err = UStageTableHelper::GetStageLastWave(Stage->Level, LastWave); !GameCore::IsOK(Err))
	{
		return Err;
	}

	StagePtr = Stage;
	NextWave = Stage->Wave;

	return GameCore::Pass();
}

FGErrorInfo UStageWaveComponent::Start()
{
	auto Stage = GetStage();
	if (!Stage.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("Start():Stage를 찾을 수 없습니다."));
	}

	if (NextWave > LastWave)
	{
		return GameCore::Throw(GameErr::Stage::LAST_WAVE);
	}

	TArray<UStageWaveContext*> WaveContexts;
	if (auto Err = UStageTableHelper::GetStageWaveContexts(Stage->Level, WaveContexts); !GameCore::IsOK(Err))
	{
		return Err;
	}

	if ((NextWave + 1) >= WaveContexts.Num())
	{
		auto Msg = FString::Printf(TEXT("[NextWave:%d][WaveContextCount:%d]"), (NextWave + 1), WaveContexts.Num());
		return GameCore::Throw(GameErr::Stage::NO_NEXT_WAVE, Msg);
	}

	NextWave++;

	auto WaveContext = WaveContexts[NextWave];

	// NOTE. 다음 웨이브 딜레이 시간 증가
	NextWaveDelay += WaveContext->WaveInfo.DelayTime.GetTotalSeconds();

	for (auto& MonsterGroupInfo : WaveContext->MonsterGroupInfos)
	{
		FStageSpawnData NewData;
		NewData.UniqueId = ++SequenceId;
		NewData.Amount = MonsterGroupInfo.Amount;
		NewData.AmountValue = MonsterGroupInfo.AmountValue;
		NewData.AmountDelayTime = MonsterGroupInfo.AmountDelayTime.GetTotalSeconds();
		NewData.Spawner = MonsterGroupInfo.Spawner;
		NewData.Path = MonsterGroupInfo.Path;
		NewData.MonsterContext = MonsterGroupInfo.MonsterContext;

		SpawnDatas.Emplace(NewData);
	}

	return GameCore::Pass();
}

FGErrorInfo UStageWaveComponent::K2_GetStage(UPARAM(DisplayName = "ReturnValue") FStage& CurrentStage) const
{
	if (!StagePtr.IsValid())
	{
		CurrentStage = FStage{};
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("K2_GetStage():Stage를 찾을 수 없습니다."));
	}
	CurrentStage = *StagePtr.Pin();
	return GameCore::Pass();
}

TSharedPtr<FStage> UStageWaveComponent::GetStage() const
{
	if (!StagePtr.IsValid())
	{
		return nullptr;
	}
	return StagePtr.Pin();
}

bool UStageWaveComponent::IsNextWaveLocked() const
{
	return NextWaveDelay > 0.0;
}

bool UStageWaveComponent::IsPlaying() const
{
	return SpawnDatas.Num() > 0;
}

bool UStageWaveComponent::IsComplete() const
{
	return NextWave > LastWave;
}

int32 UStageWaveComponent::GetWave() const
{
	if (!StagePtr.IsValid())
	{
		return INDEX_NONE;
	}

	return StagePtr.Pin()->Wave;
}

void UStageWaveComponent::OnSpawn(FStageSpawnParam Params)
{
	if (!RequestStageSpawnEvent.IsBound())
	{
		UE_LOGFMT(LogStage, Warning, "OnSpawn(Params):RequestStageSpawnEvent가 바인딩되어 있지 않습니다.");
		return;
	}

	RequestStageSpawnEvent.Execute(Params);
}
