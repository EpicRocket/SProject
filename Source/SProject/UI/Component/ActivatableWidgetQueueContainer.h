


#pragma once

#include "Blueprint/UserWidgetPool.h"
#include "ActivatableWidgetContainerInterface.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "GameplayTagContainer.h"

#include "ActivatableWidgetQueueContainer.generated.h"

class UCommonActivatableWidget;

UCLASS()
class UActivatableWidgetQueueContainer : public UCommonActivatableWidgetQueue, public IActivatableWidgetContainerInterface
{
    GENERATED_BODY()

public:
    // IActivatableWidgetContainerInterface
    virtual UCommonActivatableWidget* AddWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, TFunctionRef<void(UCommonActivatableWidget &)> InstanceInitFunc) override;
    virtual void RemoveWidget(UCommonActivatableWidget &WidgetToRemove) override;
    virtual void ClearWidgets() override;
    virtual int32 GetNumWidgets() const override;
    virtual UCommonActivatableWidget *GetActiveWidget() const override;
    // ~IActivatableWidgetContainerInterface

protected:
    virtual void SynchronizeProperties() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "UI.Layer"))
    FGameplayTag LayerName;

private:
    UPROPERTY(EditAnywhere, Category = Content)
    TSubclassOf<UCommonActivatableWidget> RootContentWidgetClass;

    UPROPERTY(Transient)
    TObjectPtr<UCommonActivatableWidget> RootContentWidget;
};
