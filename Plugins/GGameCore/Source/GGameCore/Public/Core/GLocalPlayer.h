
#pragma once

#include "Engine/LocalPlayer.h"

#include "GLocalPlayer.generated.h"

class AGPlayerController;
class APlayerState;
class APawn;

UCLASS(Transient)
class GGAMECORE_API UGLocalPlayer : public ULocalPlayer
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UGLocalPlayer* LocalPlayer, AGPlayerController* PlayerController);
    FPlayerControllerSetDelegate OnPlayerControllerSet;

    DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UGLocalPlayer* LocalPlayer, APlayerState* PlayerState);
    FPlayerStateSetDelegate OnPlayerStateSet;

    DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerPawnSetDelegate, UGLocalPlayer* LocalPlayer, APawn* Pawn);
    FPlayerPawnSetDelegate OnPlayerPawnSet;
};
