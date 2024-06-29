// File generate: 2024.06.30-02.56.04
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"
#include "ResourceTable.generated.h"


UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None = 0,
	Gold = 1,
	Cash = 2,
	Item = 3,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EResourceType, EResourceType::Max)

