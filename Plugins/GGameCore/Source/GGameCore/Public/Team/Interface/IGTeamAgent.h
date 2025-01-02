
#pragma once

#include "GenericTeamAgentInterface.h"

#include "IGTeamAgent.generated.h"

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class GGAMECORE_API UGTeamAgent : public UGenericTeamAgentInterface
{
    GENERATED_BODY()
};

class GGAMECORE_API IGTeamAgent : public IGenericTeamAgentInterface
{
    GENERATED_BODY()
};
