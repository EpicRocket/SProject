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

bool UStageWaveComponent::IsPlaying() const
{
	// TODO:
	return false;
}

bool UStageWaveComponent::IsComplete() const
{
	return false;
}

int32 UStageWaveComponent::GetWave() const
{
	if (!StagePtr.IsValid())
	{
		return INDEX_NONE;
	}

	return StagePtr.Pin()->Wave;
}

//FGErrorInfo UStageWaveComponent::SetWaveGroup(int32 TargetWaveGroup)
//{
//	WaveGroup = TargetWaveGroup;
//	UStageTableHelper::GetWaveGroupInfo(WaveGroup, WaveGroupInfo);
//	CurrentWaveIndex = 0;
//	return FGErrorInfo();
//}
//
//FGErrorInfo UStageWaveComponent::WaveStart()
//{
//	OnWaveStart();
//	return FGErrorInfo();
//}
//
//FGErrorInfo UStageWaveComponent::WaveEnd()
//{
//
//	OnWaveEnd();
//	return FGErrorInfo();
//}
//
//FGErrorInfo UStageWaveComponent::NextWave()
//{
//	if (CurrentWaveIndex < WaveGroupInfo.Num())
//	{
//		return FGErrorInfo();
//	}
//
//	auto CurrentWaveGroup = WaveGroupInfo[CurrentWaveIndex];
//	if (CurrentWaveGroup.Type != 2 && CurrentWaveIndex < WaveGroupInfo.Num() - 1)
//	{
//		CurrentWaveIndex += 1;
//	}
//	else
//	{
//		OnStageWaveComplete();
//	}
//
//	return FGErrorInfo();
//}
//
//TArray<FStageWaveGroupInfo> UStageWaveComponent::GetWaveGroupInfos()
//{
//	return WaveGroupInfo;
//}
//
//TArray<FMonsterGroupTableRow> UStageWaveComponent::GetCurrentMonsterGroupInfo()
//{
//	TArray<FMonsterGroupTableRow> Info;
//	UStageTableHelper::GetMonsterGroupInfo(WaveGroupInfo[CurrentWaveIndex].MonsterGroup, Info);
//	return Info;
//}
//
//TArray<FMonsterGroupTableRow> UStageWaveComponent::GetMonsterGroupInfos(int32 MonsterGroup)
//{
//	TArray<FMonsterGroupTableRow> Info;
//	UStageTableHelper::GetMonsterGroupInfo(MonsterGroup, Info);
//	return Info;
//}
