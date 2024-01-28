

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPtr.h"

#include "AsyncAction_PushContentToLayer.generated.h"

class APlayerController;
class UCustomActivatableWidget;
class UObject;
struct FFrame;
struct FStreamableHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPushContentToLayerAsyncDelegate, UCustomActivatableWidget*, NewWidget);

UCLASS(BlueprintType)
class UAsyncAction_PushContentToLayer : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	virtual void Cancel() override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_PushContentToLayer* PushContentToLayer(APlayerController* InOwningPlayer, UPARAM(meta = (AllowAbstract=false)) TSoftClassPtr<UCustomActivatableWidget> InWidgetClass, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag InLayerName, bool bInSuspendInputUntilComplete = true);

	virtual void Activate() override;

public:
	UPROPERTY(BlueprintAssignable)
	FPushContentToLayerAsyncDelegate BeforePush;

	UPROPERTY(BlueprintAssignable)
	FPushContentToLayerAsyncDelegate AfterPush;

private:
	FGameplayTag LayerName;
	bool bSuspendInputUntilComplete = false;
	TWeakObjectPtr<APlayerController> OwningPlayerPtr;
	TSoftClassPtr<UCustomActivatableWidget> WidgetClass;

	TSharedPtr<FStreamableHandle> StreamingHandle;
};
