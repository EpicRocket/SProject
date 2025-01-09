
#pragma once

#include "GenericTeamAgentInterface.h"

#include "IGTeamAgent.generated.h"

struct FGTeamTracker;

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class GGAMECORE_API UGTeamAgent : public UGenericTeamAgentInterface
{
    GENERATED_BODY()
};

class GGAMECORE_API IGTeamAgent : public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetTeamID"))
	virtual uint8 K2_GetTeamID() const { return GetGenericTeamId(); }

    UFUNCTION(BlueprintCallable)
    virtual FGTeamTracker GetTeamTracker() const;
};
