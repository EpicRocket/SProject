

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"

#include "AsyncAction_ClearContentToSubLayer.generated.h"

class APlayerController;
class USubActivatableWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClearContentToSubLayerAsyncDelegate);

UCLASS(BlueprintType)
class UAsyncAction_ClearContentToSubLayer : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_ClearContentToSubLayer* ClearContentToSubLayer(USubActivatableWidget* ParentWidget, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTagContainer InSubLayerNames);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_ClearContentToSubLayer* ClearContentToTopSubLayer(APlayerController* InOwningPlayer, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag InLayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTagContainer InSubLayerNames);

	virtual void Activate() override;

private:
	void ExecuteInternal();

public:
	UPROPERTY(BlueprintAssignable) 
	FClearContentToSubLayerAsyncDelegate OnComplete;

	UPROPERTY(BlueprintAssignable)
	FClearContentToSubLayerAsyncDelegate OnCancel;

private:
	FGameplayTagContainer LayerNames;
	TWeakObjectPtr<USubActivatableWidget> ParentWidgetPtr;
	TArray<TFunction<void(TFunction<void(bool)>)>> TaskQueue;
};
