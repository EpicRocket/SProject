
#pragma once

#include "LatentActions.h"

class UWorld;
class APlayerController;
struct FGErrorInfo;

class GGAMECORE_API FGGameLoadAction : public FPendingLatentAction
{
public:
	FGGameLoadAction(const FLatentActionInfo& InLatentInfo, UWorld* World, TFunction<void(APlayerController*)> CompletedCallback, TFunction<void(FGErrorInfo)> FailedCallback)
		: LatentInfo(InLatentInfo)
		, WorldPtr(World)
		, OnCompleted(CompletedCallback)
		, OnFailed(FailedCallback)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override;

#if WITH_EDITOR
	virtual FString GetDescription() const override;
#endif

private:
	FLatentActionInfo LatentInfo;
	TWeakObjectPtr<UWorld> WorldPtr;
	TFunction<void(APlayerController*)> OnCompleted;
	TFunction<void(FGErrorInfo)> OnFailed;
};
