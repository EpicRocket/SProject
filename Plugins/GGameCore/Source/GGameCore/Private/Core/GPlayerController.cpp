// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Core/GPlayerController.h"
// include Engine
#include "Components/GameFrameworkComponentManager.h"
// include GameCore
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

		SetGenericTeamId(LocalPlayer->GetLocalPlayerIndex());
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

void AGPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void AGPlayerController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	TeamID = InTeamID;
}

FGenericTeamId AGPlayerController::GetGenericTeamId() const
{
	return TeamID;
}
