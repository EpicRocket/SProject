
#pragma once

#include "GameFramework/PlayerController.h"
#include "Team/Interface/IGTeamAgent.h"

#include "GPlayerController.generated.h"

class APawn;

UCLASS(Blueprintable)
class GGAMECORE_API AGPlayerController : public APlayerController, public IGTeamAgent
{
    GENERATED_BODY()

public:
	virtual void ReceivedPlayer() override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void OnPossess(class APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
