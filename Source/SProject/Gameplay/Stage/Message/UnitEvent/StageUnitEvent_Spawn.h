#pragma once

#include "Gameplay/Stage/Message/StageUnitEventMessage.h"

#include "StageUnitEvent_Spawn.generated.h"

namespace Stage
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Unit_Spawn);
}

UCLASS(BlueprintType)
class MY_API UStageUnitEvent_Spawn : public UStageUnitEvent
{
    GENERATED_BODY()

public:
	virtual FGameplayTag GetEventTag() const override;

};
