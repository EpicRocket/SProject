
#pragma once

#include "UObject/Interface.h"

#include "GTeamAgentInterface.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class GGAMECORE_API UGTeamAgentInterface : public UInterface
{
    GENERATED_BODY()
};

class GGAMECORE_API IGTeamAgentInterface : public IInterface
{
    GENERATED_BODY()
public:
    
};
