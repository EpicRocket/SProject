

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"

#include "AsyncAction_PopContentToSubLayer.generated.h"

class APlayerController;
class USubActivatableWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPopContentToSubLayerAsyncDelegate);

UCLASS(BlueprintType)
class UAsyncAction_PopContentToSubLayer : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_PopContentToSubLayer* PopContentToSubLayer (USubActivatableWidget* ParentWidget, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName);

	virtual void Activate() override;

public:
	UPROPERTY(BlueprintAssignable)
	FPopContentToSubLayerAsyncDelegate OnComplete;

	UPROPERTY(BlueprintAssignable)
	FPopContentToSubLayerAsyncDelegate OnCancel;

private:
	FGameplayTag LayerName;
	TWeakObjectPtr<USubActivatableWidget> ParentWidgetPtr;
	TFunction<void(TFunction<void(bool)>)> Task;
};