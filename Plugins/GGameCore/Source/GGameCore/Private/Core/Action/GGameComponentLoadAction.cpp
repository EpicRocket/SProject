
#include "Core/Action/GGameComponentLoadAction.h"
#include "Engine/World.h"
// inclue GameCore
#include "Error/GErrorTypes.h"
#include "Error/GErrorManager.h"

void FGGameComponentLoadAction::UpdateOperation(FLatentResponse& Response)
{
    if (!WorldPtr.IsValid())
    {
        OnFailed(GameCore::Throw(GameErr::WORLD_INVALID, TEXT("FGGameComponentLoadAction")));
        Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
        return;
    }

    if (!ComponentPtr.IsValid())
    {
        OnFailed(GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("FGGameComponentLoadAction")));
        Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
        return;
    }

    if (ComponentPtr->HasBegunPlay())
    {
        OnCompleted();
		Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
    }
}

#if WITH_EDITOR
FString FGGameComponentLoadAction::GetDescription() const
{
	return TEXT("Waiting for Component to load.");
}
#endif
