
#pragma once

#include "GameFramework/Character.h"
#include "Team/Interface/GTeamAgentInterface.h"

#include "UnitCharacter.generated.h"

UCLASS()
class GGAMECORE_API AUnitCharacter : public ACharacter, public IGTeamAgentInterface
{
    GENERATED_BODY()

public:
    AUnitCharacter();

public:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    FGuid UnitId;
};
