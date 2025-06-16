// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageMonsterTypes.h"
// include Engine
#include "Engine/Texture2D.h"
#include "BehaviorTree/BehaviorTree.h"
// include Project
#include "Gameplay/Stage/Unit/StageMonsterUnit.h"

void UStageMonsterContext::Load()
{
	MonsterInfo.UnitClass = MonsterInfo.UnitClassPtr.Get();
	MonsterInfo.Icon = MonsterInfo.IconPtr.Get();
	MonsterInfo.AI = MonsterInfo.AIPtr.Get();
}
