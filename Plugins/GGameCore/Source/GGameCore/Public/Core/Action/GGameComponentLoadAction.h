
#pragma once

#include "LatentActions.h"
#include "Engine/LatentActionManager.h"

class UWorld;
class UActorComponent;
struct FGErrorInfo;

class GGAMECORE_API FGGameComponentLoadAction : public FPendingLatentAction
{
public:
	FGGameComponentLoadAction(const FLatentActionInfo& InLatentInfo, UWorld* World, UActorComponent* TargetComponent, TFunction<void()> CompletedCallback, TFunction<void(FGErrorInfo)> FailedCallback);

	virtual void UpdateOperation(FLatentResponse& Response) override;

#if WITH_EDITOR
	virtual FString GetDescription() const override;
#endif

private:
	FLatentActionInfo LatentInfo;
	TWeakObjectPtr<UWorld> WorldPtr;
	TWeakObjectPtr<UActorComponent> ComponentPtr;
	TFunction<void()> OnCompleted;
	TFunction<void(FGErrorInfo)> OnFailed;
};
