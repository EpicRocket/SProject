

#include "ActivatableWidgetStackContainer.h"
#include "CommonActivatableWidget.h"

UCommonActivatableWidget* UActivatableWidgetStackContainer::AddWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, TFunctionRef<void(UCommonActivatableWidget &)> InstanceInitFunc)
{
    return Super::AddWidget<UCommonActivatableWidget>(WidgetClass, InstanceInitFunc);
}

void UActivatableWidgetStackContainer::RemoveWidget(UCommonActivatableWidget &WidgetToRemove)
{
    Super::RemoveWidget(WidgetToRemove);
}

void UActivatableWidgetStackContainer::ClearWidgets()
{
    Super::ClearWidgets();
}

int32 UActivatableWidgetStackContainer::GetNumWidgets() const
{
    return Super::GetNumWidgets();
}

UCommonActivatableWidget* UActivatableWidgetStackContainer::GetActiveWidget() const
{
    return Super::GetActiveWidget();
}
