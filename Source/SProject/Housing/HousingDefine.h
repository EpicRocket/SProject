#pragma once

#include "HousingDefine.generated.h"

UENUM(BlueprintType)
enum class EHousingModeType : uint8
{
	Player,
	Editor,
	Spectator,
};

UENUM(BlueprintType)
enum class EEnvironmentPropType : uint8
{
	Block,
	Carpet,

};

