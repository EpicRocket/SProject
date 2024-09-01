
#include "Core/GGameInstance.h"
// Engine
#include "Engine/LocalPlayer.h"

#include "UI/GUIManagerSubsystem.h"
#include "Core/GLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameInstance)


int32 UGGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	UGLocalPlayer* GPlayer = Cast<UGLocalPlayer>(NewPlayer);
	if (GPlayer)
	{
		GPlayer->UniqueId = 255;
		for (auto& [Id, Player] : LocalPlayers)
		{
			if (!Player.IsValid())
			{
				GPlayer->UniqueId = Id;
			}
		}

		if (GPlayer->UniqueId == 255)
		{
			GPlayer->UniqueId = LocalPlayers.Num();
			LocalPlayers.Emplace(GPlayer->UniqueId, nullptr);
		}
	}

	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
		LocalPlayers[GPlayer->UniqueId] = GPlayer;

		if (GPlayer->UniqueId == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("AddLocalPlayer: Set %s to Primary Player"), *NewPlayer->GetName());
		}

		GetSubsystem<UGUIManagerSubsystem>()->NotifyPlayerAdded(GPlayer);
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

	if (GPlayer->UniqueId == 0)
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
