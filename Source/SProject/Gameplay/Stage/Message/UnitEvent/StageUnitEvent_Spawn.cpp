// Team EpicRocket

#include "StageUnitEvent_Spawn.h"

namespace Stage
{
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Unit_Spawn, "Gameplay.Stage.Unit.Spawn");
}

FGameplayTag UStageUnitEvent_Spawn::GetEventTag() const
{
	return Stage::Tag_Gameplay_Stage_Unit_Spawn;
}
