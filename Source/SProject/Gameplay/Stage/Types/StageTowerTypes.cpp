// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageTowerTypes.h"
// include Engine
#include "Engine/Texture2D.h"
#include "BehaviorTree/BehaviorTree.h"
// include Project
#include "Gameplay/Stage/Unit/StageTowerUnit.h"

void UStageTowerContext::Load()
{
	TowerInfo.UnitClass = TowerInfo.UnitClassPtr.Get();
	TowerInfo.Icon = TowerInfo.IconPtr.Get();
	TowerInfo.AI = TowerInfo.AIPtr.Get();
}
