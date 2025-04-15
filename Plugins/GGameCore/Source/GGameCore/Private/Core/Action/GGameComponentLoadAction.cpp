
#include "Core/Action/GGameComponentLoadAction.h"
#include "Engine/World.h"
// inclue GameCore
#include "Error/GErrorTypes.h"
#include "Error/GErrorManager.h"

FGGameComponentLoadAction::FGGameComponentLoadAction(const FLatentActionInfo& InLatentInfo, UWorld* World, UActorComponent* TargetComponent, TFunction<void()> CompletedCallback, TFunction<void(FGErrorInfo)> FailedCallback)
	: LatentInfo(InLatentInfo)
	, WorldPtr(World)
	, ComponentPtr(TargetComponent)
	, OnCompleted(CompletedCallback)
	, OnFailed(FailedCallback)
{
}

void FGGameComponentLoadAction::UpdateOperation(FLatentResponse& Response)
{
	if (!WorldPtr.IsValid())
	{
		if (OnFailed)
		{
			OnFailed(GameCore::Throw(GameErr::WORLD_INVALID, TEXT("FGGameComponentLoadAction")));
		}
		Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
		return;
	}

	if (!ComponentPtr.IsValid())
	{
		if (OnFailed)
		{
			OnFailed(GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("FGGameComponentLoadAction")));
		}
		Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
		return;
	}

	if (ComponentPtr->HasBegunPlay())
	{
		if (OnCompleted)
		{
			OnCompleted();
		}
		Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
	}
}

#if WITH_EDITOR
FString FGGameComponentLoadAction::GetDescription() const
{
	return TEXT("Waiting for Component to load.");
}
#endif
