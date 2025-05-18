
#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "StageWaveEventMessage.generated.h"

namespace Stage
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Wave_Start);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Wave_Pause);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Wave_End);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Wave_Stop);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Wave_Spawn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Wave_Clear);
}

USTRUCT(BlueprintType)
struct FStageWaveStartMessage
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FStageWaveSpawnMessage
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnMonsterIndex = 0;
};

USTRUCT(BlueprintType)
struct FStageWaveEndMessage
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FStageWaveStopMessage
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FStageWaveClearMessage
{
	GENERATED_BODY()
};