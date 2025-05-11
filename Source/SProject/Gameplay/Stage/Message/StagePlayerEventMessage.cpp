
#include "StagePlayerEventMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePlayerEventMessage)

namespace Stage
{
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_NewStart, "Gameplay.Stage.Player.NewStart");
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_Restart, "Gameplay.Stage.Player.Restart");
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_Continue, "Gameplay.Stage.Player.Continue");

	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_Hp_Changed, "Gameplay.Stage.Player.Hp.Changed");
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_UsePoint_Changed, "Gameplay.Stage.Player.UsePoint.Changed");

	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_StartPoint, "Gameplay.Stage.Player.StartPoint");

	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_Wave_Start, "Gameplay.Stage.Player.Wave.Start");
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_Wave_Pause, "Gameplay.Stage.Player.Wave.Pause");
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_Wave_End, "Gameplay.Stage.Player.Wave.End");
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_Wave_Stop, "Gameplay.Stage.Player.Wave.Stop");
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Player_Wave_Spawn, "Gameplay.Stage.Player.Wave.Spawn");
}
