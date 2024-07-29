
#pragma once

#include "Engine/LevelScriptActor.h"

#include "StageLevel.generated.h"

class ABuildZone;

UCLASS()
class SPROJECT_API AStageLevel : public ALevelScriptActor
{
    GENERATED_BODY()

public:
    TMap<int32, TWeakObjectPtr<ABuildZone>> BuildZones;
};
