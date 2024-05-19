
#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"

#include "PrimaryGameLayout.generated.h"

class UCommonActivatableWidgetContainerBase;

UCLASS(Abstract, meta = (DisableNativeTick))
class GGAMECORE_API UPrimaryGameLayout : public UCommonActivatableWidget
{
	GENERATED_BODY()

	UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerName);

protected:
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};
