
#include "Core/Action/GGameLoadAction.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LatentActionManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
// inclue GameCore
#include "Error/GErrorTypes.h"
#include "Error/GErrorManager.h"

void FGGameLoadAction::UpdateOperation(FLatentResponse& Response)
{
	if (!WorldPtr.IsValid())
	{
		OnFailed(GameCore::Throw(GameErr::WORLD_INVALID, TEXT("FGGameLoadAction")));
		Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
		return;
	}

	APlayerController* PrimaryController = nullptr;
	for (FConstPlayerControllerIterator Iterator = WorldPtr->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* NextPlayer = Iterator->Get();
		if (NextPlayer && NextPlayer->PlayerState && NextPlayer->IsPrimaryPlayer() && NextPlayer->PlayerState->GetUniqueId().IsValid())
		{
			PrimaryController = NextPlayer;
			break;
		}
	}

	if (!PrimaryController)
	{
		return;
	}
	else if (!PrimaryController->HasActorBegunPlay())
	{
		return;
	}

	OnCompleted(PrimaryController);
	Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
}

#if WITH_EDITOR
FString FGGameLoadAction::GetDescription() const
{
	return TEXT("Waiting for PlayerController to load.");
}
#endif
