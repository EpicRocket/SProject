
#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "StagePlayerEventMessage.generated.h"

namespace Stage
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_Health_Changed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Player_UsePoint_Changed);
}

USTRUCT(BlueprintType)
struct FStagePlayerHealthMessage
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
