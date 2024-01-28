

#pragma once

#include "Blueprint/UserWidgetPool.h"
#include "ActivatableWidgetContainerInterface.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "GameplayTagContainer.h"

#include "ActivatableWidgetStackContainer.generated.h"

class UCommonActivatableWidget;

UCLASS()
class UActivatableWidgetStackContainer : public UCommonActivatableWidgetStack, public IActivatableWidgetContainerInterface
{
    GENERATED_BODY()

public:
    // IActivatableWidgetContainerInterface
    virtual UCommonActivatableWidget *AddWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, TFunctionRef<void(UCommonActivatableWidget &)> InstanceInitFunc) override;
    virtual void RemoveWidget(UCommonActivatableWidget &WidgetToRemove) override;
    virtual void ClearWidgets() override;
    virtual int32 GetNumWidgets() const override;
    virtual UCommonActivatableWidget *GetActiveWidget() const override;
    // ~IActivatableWidgetContainerInterface
};
