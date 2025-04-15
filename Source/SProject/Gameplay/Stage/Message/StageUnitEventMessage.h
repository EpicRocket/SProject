
#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "StageUnitEventMessage.generated.h"

namespace Stage
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Unit_Attack_Player);
}

class AStageUnitCharacter;

USTRUCT(BlueprintType)
struct MY_API FStageUnitAttackPlayerMessage
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<AStageUnitCharacter> SourceUnit;

};
