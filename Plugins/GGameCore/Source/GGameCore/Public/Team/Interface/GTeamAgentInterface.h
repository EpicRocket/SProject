
#pragma once

#include "GenericTeamAgentInterface.h"

#include "GTeamAgentInterface.generated.h"

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class GGAMECORE_API UGTeamAgentInterface : public UGenericTeamAgentInterface
{
    GENERATED_BODY()
};

class GGAMECORE_API IGTeamAgentInterface : public IGenericTeamAgentInterface
{
    GENERATED_BODY()
};
