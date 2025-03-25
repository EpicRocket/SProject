
#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "StagePlayerEventMessage.generated.h"

namespace Stage
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_NewStart);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_Restart);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_Continue);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_Hp_Changed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_UsePoint_Changed);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_StartPoint);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_Wave_Start);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_Wave_Pause);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_Wave_End);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_Wave_Spawn);
}

USTRUCT(BlueprintType)
struct FStagePlayerNewStartMessage
{
	GENERATED_BODY()

};

USTRUCT(BlueprintType)
struct FStagePlayerRestartMessage
{
	GENERATED_BODY()

};

USTRUCT(BlueprintType)
struct FStagePlayerContinueMessage
{
	GENERATED_BODY()

};

USTRUCT(BlueprintType)
struct FStagePlayerHpMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OldValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NewValue = 0;
};

USTRUCT(BlueprintType)
struct FStagePlayerUsePointMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OldValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NewValue = 0;
};

USTRUCT(BlueprintType)
struct FStagePlayerStartPointMessage
{
	GENERATED_BODY()

};

USTRUCT(BlueprintType)
struct FStagePlayerWaveStartMessage
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FStagePlayerWaveSpawnMessage
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnMonsterIndex = 0;
};