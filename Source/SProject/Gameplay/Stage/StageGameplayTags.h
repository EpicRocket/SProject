
#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "StageGameplayTags.generated.h"

namespace Stage
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Request_Build)
}

USTRUCT(BlueprintType)
struct FStageRequestBuildMessage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Position = INDEX_NONE;
};
