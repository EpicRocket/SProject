#include "StageWaveComponent.h"

#include "Gameplay/GameplayUserPlayer.h"
#include "User/StageSubsystem.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/StageLogging.h"

void UStageWaveComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Paused = true;
}

FGErrorInfo UStageWaveComponent::SetWaveGroup(int32 TargetWaveGroup)
{
	WaveGroup = TargetWaveGroup;
	//UStageTableHelper::GetWaveGroupInfo(WaveGroup, WaveGroupInfo);
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
	auto CurrentWaveGroup = WaveGroupInfo[CurrentWaveIndex];
	if (CurrentWaveGroup.Type != 2 && CurrentWaveIndex < WaveGroupInfo.Num() - 1)
	{
		CurrentWaveIndex += 1;
	}
	else {
		OnStageWaveComplete();
	}

	return FGErrorInfo();
}

TArray<FStageWaveGroupInfo> UStageWaveComponent::GetWaveGroupInfo()
{
	return WaveGroupInfo;
}

TArray<FMonsterGroupTableRow> UStageWaveComponent::GetCurrentMonsterGroupInfo()
{
	TArray<FMonsterGroupTableRow> Info;
	//UStageTableHelper::GetMonsterGroupInfo(WaveGroupInfo[CurrentWaveIndex].MonsterGroup, Info);
	return Info;
}
