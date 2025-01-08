
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Team/GTeamTypes.h"

#include "GTeamSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class GGAMECORE_API UGTeamSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UGTeamSettings();

public:
    UPROPERTY(Config, EditDefaultsOnly, Category = "Team")
    FGTeamTracker DefaultTeamTracker;

};
