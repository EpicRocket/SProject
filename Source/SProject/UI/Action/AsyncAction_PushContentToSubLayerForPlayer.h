
#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPtr.h"

#include "AsyncAction_PushContentToSubLayerForPlayer.generated.h"

class UCommonActivatableWidget;
class APlayerController;

struct FStreamableHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPushContentToSubLayerForPlayerAsyncDelegate, UCommonActivatableWidget*, UserWidget);

UCLASS(BlueprintType)
class UAsyncAction_PushContentToSubLayerForPlayer : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	virtual void Cancel() override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_PushContentToSubLayerForPlayer* PushContentToSubLayer_ForPlayer(APlayerController* OwningPlayer, UPARAM(meta = (AllowAbstract = false)) TSoftClassPtr<UCommonActivatableWidget> WidgetClass, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName, bool bSuspendInputUntilComplete = true);

	virtual void Activate() override;

public:
	UPROPERTY(BlueprintAssignable)
	FPushContentToSubLayerForPlayerAsyncDelegate BeforePush;

	UPROPERTY(BlueprintAssignable)
	FPushContentToSubLayerForPlayerAsyncDelegate AfterPush;

private:
	FGameplayTag LayerName;
	FGameplayTag SubLayerName;
	bool bSuspendInputUntilComplete = false;
	TWeakObjectPtr<APlayerController> OwningPlayerPtr;
	TSoftClassPtr<UCommonActivatableWidget> WidgetClass;

	TSharedPtr<FStreamableHandle> StreamingHandle;
};
