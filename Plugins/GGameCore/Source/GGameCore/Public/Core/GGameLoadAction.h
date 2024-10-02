
#pragma once

#include "LatentActions.h"

class UWorld;

class GGAMECORE_API FGGameLoadAction : public FPendingLatentAction
{

public:
	FGGameLoadAction(const FLatentActionInfo& InLatentInfo, UWorld* World, TFunction<void()> CompletedCallback, TFunction<void()> FailedCallback)
		: LatentInfo(InLatentInfo)
		, WorldPtr(World)
		, OnCompleted(CompletedCallback)
		, OnFailed(FailedCallback)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override;

#if WITH_EDITOR
	virtual FString GetDescription() const override;
#endif;

private:
	FLatentActionInfo LatentInfo;
	TWeakObjectPtr<UWorld> WorldPtr;
	TFunction<void()> OnCompleted;
	TFunction<void()> OnFailed;
};
