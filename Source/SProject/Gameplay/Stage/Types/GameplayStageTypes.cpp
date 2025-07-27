// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "GameplayStageTypes.h"
// include Engine
#include "Engine/Texture2D.h"
#include "BehaviorTree/BehaviorTree.h"
// include Project
#include "Gameplay/Stage/Unit/StageMonsterUnit.h"
#include "Gameplay/Stage/Unit/StageTowerUnit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayStageTypes)

void UStageContext::Load()
{
	StageInfo.Icon = StageInfo.IconPtr.Get();

	TArray<UGContext*> Contexts;
	Contexts.Append(WaveContexts);
	Contexts.Append(RepaetWaveContexts);
	Contexts.Append(TowerContexts);


	for (auto& Context : Contexts)
	{
		if (!IsValid(Context))
		{
			Context->Load();
		}
	}
}

void UStageWaveContext::Load()
{
	for (auto& MonsterGroupInfo : MonsterGroupInfos)
	{
		if (!IsValid(MonsterGroupInfo.MonsterContext))
		{
			continue;
		}
		MonsterGroupInfo.MonsterContext->Load();
	}
}

void UStageMonsterContext::Load()
{
	MonsterInfo.UnitClass = MonsterInfo.UnitClassPtr.Get();
	MonsterInfo.Icon = MonsterInfo.IconPtr.Get();
	MonsterInfo.AI = MonsterInfo.AIPtr.Get();
}

void UStageTowerContext::Load()
{
	TowerInfo.UnitClass = TowerInfo.UnitClassPtr.Get();
	TowerInfo.Icon = TowerInfo.IconPtr.Get();
	TowerInfo.AI = TowerInfo.AIPtr.Get();
}
