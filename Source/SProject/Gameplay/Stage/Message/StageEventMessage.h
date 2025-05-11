
#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "StageEventMessage.generated.h"

namespace Stage
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Transition);
} // namespace Stage

USTRUCT(BlueprintType)
struct FStageTransitionMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;
};