﻿
#include "Core/GPlayerController.h"

#include "Core/GLocalPlayer.h"

void AGPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (UGLocalPlayer* LocalPlayer = Cast<UGLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);
		if (PlayerState)
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}

void AGPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (UGLocalPlayer* LocalPlayer = Cast<UGLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, InPawn);
	}
}

void AGPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UGLocalPlayer* LocalPlayer = Cast<UGLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, InPawn);
	}
}

void AGPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (UGLocalPlayer* LocalPlayer = Cast<UGLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, nullptr);
	}
}