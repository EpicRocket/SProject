
#include "MyLocalPlayer.h"

#include "Engine/GameInstance.h"
#include "GameFramework/Pawn.h"
#include "MyPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyLocalPlayer)

FDelegateHandle UMyLocalPlayer::CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate)
{
	if (APlayerController* PC = GetPlayerController(GetWorld()))
	{
		Delegate.Execute(this, PC);
	}
	return OnPlayerControllerSet.Add(Delegate);
}

FDelegateHandle UMyLocalPlayer::CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate)
{
	APlayerController* PC = GetPlayerController(GetWorld());
	APlayerState* PlayerState = PC ? PC->PlayerState : nullptr;
	if (PlayerState)
	{
		Delegate.Execute(this, PlayerState);
	}
	return OnPlayerStateSet.Add(Delegate);
}

FDelegateHandle UMyLocalPlayer::CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate)
{
	APlayerController* PC = GetPlayerController(GetWorld());
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (Pawn)
	{
		Delegate.Execute(this, Pawn);
	}
	return OnPlayerPawnSet.Add(Delegate);
}

bool UMyLocalPlayer::GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData, int32 StereoViewIndex) const
{
    if (!bIsPlayerViewEnabled)
	{
		return false;
	}
	return Super::GetProjectionData(Viewport, ProjectionData, StereoViewIndex);
}
