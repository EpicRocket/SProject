
#include "Core/GGameInstance.h"
// Engine
#include "Engine/LocalPlayer.h"

#include "UI/GUIManagerSubsystem.h"
#include "Core/GLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameInstance)


int32 UGGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	UGLocalPlayer* GPlayer = Cast<UGLocalPlayer>(NewPlayer);
	uint8 ReservedId = 255;
	if (GPlayer)
	{
		GPlayer->UniqueId = ReservedId;
		for (auto& [Id, Player] : LocalPlayers)
		{
			if (!Player.IsValid())
			{
				GPlayer->UniqueId = ReservedId = Id;
				Player = GPlayer;
			}
		}

		if (GPlayer->UniqueId == 255)
		{
			GPlayer->UniqueId = ReservedId = LocalPlayers.Num();
			LocalPlayers.Emplace(GPlayer->UniqueId, GPlayer);
		}
	}

	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
		if (GPlayer && GPlayer->UniqueId == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("AddLocalPlayer: Set %s to Primary Player"), *NewPlayer->GetName());
		}

		GetSubsystem<UGUIManagerSubsystem>()->NotifyPlayerAdded(GPlayer);
	}
	else
	{
		if (LocalPlayers.Contains(ReservedId))
		{
			LocalPlayers[ReservedId].Reset();
		}
	}
	return ReturnVal;
}

bool UGGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	UGLocalPlayer* GPlayer = Cast<UGLocalPlayer>(ExistingPlayer);

	for (auto& [Id, Player] : LocalPlayers)
	{
		if (Player == GPlayer)
		{
			Player.Reset();
			break;
		}
	}

	if (GPlayer && GPlayer->UniqueId == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("RemoveLocalPlayer: Unsetting Primary Player from %s"), *ExistingPlayer->GetName());
	}
	GetSubsystem<UGUIManagerSubsystem>()->NotifyPlayerDestroyed(GPlayer);

	return Super::RemoveLocalPlayer(ExistingPlayer);
}

UGLocalPlayer* UGGameInstance::GetPrimaryPlayer() const
{
	return GetLocalPlayer(0);
}

UGLocalPlayer* UGGameInstance::GetLocalPlayer(uint8 UniqueId) const
{
	if (LocalPlayers.Contains(UniqueId))
	{
		return LocalPlayers[UniqueId].Get();
	}
	return nullptr;
}
