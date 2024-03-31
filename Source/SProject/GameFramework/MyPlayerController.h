
#pragma once

#include "GameFramework/PlayerController.h"

#include "MyPlayerController.generated.h"

class APawn;

UCLASS(Blueprintable)
class AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void ReceivedPlayer() override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	virtual void OnRep_PlayerState();
};
