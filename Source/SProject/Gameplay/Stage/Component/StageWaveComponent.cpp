// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageWaveComponent.h"
// include GameCore
#include "Table/GTableHelper.h"
// include Project
#include "Table/StageTable.h"
#include "Gameplay/GameplayUserPlayer.h"
#include "User/StageSubsystem.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Stage.h"

void UStageWaveComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Paused = true;
}

FGErrorInfo UStageWaveComponent::Setup(int32 InStageLevel, int32 InWave)
{
	auto StageRow = UGTableHelper::GetTableData<FStageTableRow>(InStageLevel);
	if (!StageRow)
	{
		auto Msg = FString::Printf(TEXT("Setup(InStageLevel:%d, InWave:%d):Stage row isn't found."), InStageLevel, InWave);
		return GameCore::Throw(GameErr::TABLE_INVALID, Msg);
	}

	//int32 WaveGroup = StageRow->WaveGroup;


	return GameCore::Pass();
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
	return int32();
}

FGErrorInfo UStageWaveComponent::SetWaveGroup(int32 TargetWaveGroup)
{
	WaveGroup = TargetWaveGroup;
	UStageTableHelper::GetWaveGroupInfo(WaveGroup, WaveGroupInfo);
	CurrentWaveIndex = 0;
	return FGErrorInfo();
}

FGErrorInfo UStageWaveComponent::WaveStart()
{
	OnWaveStart();
	return FGErrorInfo();
}

FGErrorInfo UStageWaveComponent::WaveEnd()
{

	OnWaveEnd();
	return FGErrorInfo();
}

FGErrorInfo UStageWaveComponent::NextWave()
{
	if (CurrentWaveIndex < WaveGroupInfo.Num())
	{
		return FGErrorInfo();
	}

	auto CurrentWaveGroup = WaveGroupInfo[CurrentWaveIndex];
	if (CurrentWaveGroup.Type != 2 && CurrentWaveIndex < WaveGroupInfo.Num() - 1)
	{
		CurrentWaveIndex += 1;
	}
	else
	{
		OnStageWaveComplete();
	}

	return FGErrorInfo();
}

TArray<FStageWaveGroupInfo> UStageWaveComponent::GetWaveGroupInfos()
{
	return WaveGroupInfo;
}

TArray<FMonsterGroupTableRow> UStageWaveComponent::GetCurrentMonsterGroupInfo()
{
	TArray<FMonsterGroupTableRow> Info;
	UStageTableHelper::GetMonsterGroupInfo(WaveGroupInfo[CurrentWaveIndex].MonsterGroup, Info);
	return Info;
}

TArray<FMonsterGroupTableRow> UStageWaveComponent::GetMonsterGroupInfos(int32 MonsterGroup)
{
	TArray<FMonsterGroupTableRow> Info;
	UStageTableHelper::GetMonsterGroupInfo(MonsterGroup, Info);
	return Info;
}