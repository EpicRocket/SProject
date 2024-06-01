
#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"

#include "PrimaryGameLayout.generated.h"

class UCommonActivatableWidgetContainerBase;

UCLASS(Abstract, meta = (DisableNativeTick))
class GGAMECORE_API UPrimaryGameLayout : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UCommonActivatableWidgetContainerBase* GetLayer(FGameplayTag LayerName);

protected:
	UFUNCTION(BlueprintCallable)
	void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UCommonActivatableWidgetContainerBase* Layer);

	UFUNCTION()
	virtual void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning);

protected:
	TArray<FName> SuspendInputTokens;

	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};
