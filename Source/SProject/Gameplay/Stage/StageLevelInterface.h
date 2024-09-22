
#pragma once

#include "UObject/Interface.h"

#include "StageLevelInterface.generated.h"

UINTERFACE(BlueprintType, MinimalAPI, Category = "스테이지")
class UStageLevelInterface : public UInterface
{
    GENERATED_BODY()
};

class MY_API IStageLevelInterface : public IInterface
{
    GENERATED_BODY()
};
