
#pragma once

#include "AbilitySystem/Attribute/GAttributeSet.h"
#include "Misc/EnumRange.h"

#include "StageUnitAttributeSet.generated.h"

enum class EStageUnitAttribute : uint8
{
    None = 0,

    Max,
};
ENUM_RANGE_BY_COUNT(EStageUnitAttribute, EStageUnitAttribute::Max)

UCLASS()
class MY_API UStageUnitAttributeSet : public UGAttributeSet
{
    GENERATED_BODY()

public:

};
