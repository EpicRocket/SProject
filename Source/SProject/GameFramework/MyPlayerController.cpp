
#include "MyPlayerController.h"

#include "MyLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyPlayerController)

void AMyPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (UMyLocalPlayer* LocalPlayer = Cast<UMyLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);

		if (PlayerState)
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}

void AMyPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

    if (UMyLocalPlayer* LocalPlayer = Cast<UMyLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, InPawn);
	}
}

void AMyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

    if (UMyLocalPlayer* LocalPlayer = Cast<UMyLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, InPawn);
	}
}

void AMyPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

    if (UMyLocalPlayer* LocalPlayer = Cast<UMyLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, nullptr);
	}
}

void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PlayerState)
	{
		if (UMyLocalPlayer* LocalPlayer = Cast<UMyLocalPlayer>(Player))
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}
