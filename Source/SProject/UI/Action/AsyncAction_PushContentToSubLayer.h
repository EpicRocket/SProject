

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPtr.h"

#include "AsyncAction_PushContentToSubLayer.generated.h"

class APlayerController;
class UCustomActivatableWidget;

struct FStreamableHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPushContentToSubLayerAsyncDelegate, UCustomActivatableWidget*, NewWidget);

UCLASS(BlueprintType)
class UAsyncAction_PushContentToSubLayer : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_PushContentToSubLayer* PushContentToSubLayer(UCustomActivatableWidget* ParentWidget, UPARAM(meta = (AllowAbstract = false)) TSoftClassPtr<UCustomActivatableWidget> InWidgetClass, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, bool bInSuspendInputUntilComplete = true);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_PushContentToSubLayer* PushContentToTopSubLayer(APlayerController* InOwningPlayer, UPARAM(meta = (AllowAbstract = false)) TSoftClassPtr<UCustomActivatableWidget> InWidgetClass, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag InLayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, bool bInSuspendInputUntilComplete = true);

	virtual void Cancel() override;

	virtual void Activate() override;

public:
	UPROPERTY(BlueprintAssignable)
	FPushContentToSubLayerAsyncDelegate BeforePush;

	UPROPERTY(BlueprintAssignable)
	FPushContentToSubLayerAsyncDelegate AfterPush;

private:
	FGameplayTag LayerName;
	bool bSuspendInputUntilComplete = false;
	TWeakObjectPtr<UCustomActivatableWidget> ParentWidgetPtr;
	TSoftClassPtr<UCustomActivatableWidget> WidgetClass;

	TSharedPtr<FStreamableHandle> StreamingHandle;
};
