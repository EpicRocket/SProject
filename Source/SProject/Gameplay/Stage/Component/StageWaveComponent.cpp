#include "StageWaveComponent.h"

#include "Gameplay/Team/GameplayUserPlayer.h"
#include "User/StageSubsystem.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/StageLogging.h"

void UStageWaveComponent::OnInitialize()
{
	Paused = true;
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
	auto CurrentWaveGroup = WaveGroupInfo[CurrentWaveIndex];
	if (CurrentWaveGroup.Type != 2)
	{
		UE_LOGFMT(LogStage, Log, "웨이브 {Wave} (type: {Type}) 완료. 다음 웨이브 설정.", 
			("Wave", CurrentWaveGroup.Index), ("Type", CurrentWaveGroup.Type));
		CurrentWaveIndex += 1;
	}
	else {
		UE_LOGFMT(LogStage, Log, "웨이브 {Wave} (type: {Type}) 완료. 웨이브 반복.",
			("Wave", CurrentWaveGroup.Index), ("Type", CurrentWaveGroup.Type));
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
	UStageTableHelper::GetMonsterGroupInfo(WaveGroupInfo[CurrentWaveIndex].MonsterGroup, Info);
	return Info;
}
