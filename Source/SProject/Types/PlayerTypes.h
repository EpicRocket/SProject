
#pragma once

#include "CoreMinimal.h"

#include "PlayerTypes.generated.h"

UENUM(BlueprintType)
enum class EPlayerControlleMode : uint8
{
    None,
    PlayMode,
    TowerBuildMode,
};
