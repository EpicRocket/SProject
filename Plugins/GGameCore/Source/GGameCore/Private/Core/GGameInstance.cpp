
#include "Core/GGameInstance.h"
// Engine
#include "Engine/LocalPlayer.h"
// My
#include "UI/GUIManagerSubsystem.h"
#include "Core/GLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameInstance)


int32 UGGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
		UGLocalPlayer* GPlayer = Cast<UGLocalPlayer>(NewPlayer);

		if (!PrimaryPlayer.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("AddLocalPlayer: Set %s to Primary Player"), *NewPlayer->GetName());
			PrimaryPlayer = GPlayer;
		}

		GetSubsystem<UGUIManagerSubsystem>()->NotifyPlayerAdded(GPlayer);
	}
	return ReturnVal;
}

bool UGGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	UGLocalPlayer* GPlayer = Cast<UGLocalPlayer>(ExistingPlayer);
	if (PrimaryPlayer == GPlayer)
	{
		PrimaryPlayer.Reset();
		UE_LOG(LogTemp, Log, TEXT("RemoveLocalPlayer: Unsetting Primary Player from %s"), *ExistingPlayer->GetName());
	}
	GetSubsystem<UGUIManagerSubsystem>()->NotifyPlayerDestroyed(GPlayer);

	return Super::RemoveLocalPlayer(ExistingPlayer);
}

UGLocalPlayer* UGGameInstance::GetPrimaryPlayer() const
{
	return PrimaryPlayer.Get();
}
