
#include "GameplayUserPlayer.h"
// include Engine
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
// include GameCore
#include "Error/GErrorTypes.h"
// include Project
#include "Core/MyPlayerController.h"
#include "User/InventorySubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayUserPlayer)

APlayerController* AGameplayUserPlayer::GetOwningPlayerController() const
{
	if (!OwningPlayerController.IsValid())
	{
		return nullptr;
	}

	return OwningPlayerController.Get();
}

ULocalPlayer* AGameplayUserPlayer::GetOwningLocalPlayer() const
{
	if (!OwningPlayerController.IsValid())
	{
		return nullptr;
	}

	return OwningPlayerController->GetLocalPlayer();
}
