

#include "ActivatableWidgetQueueContainer.h"
#include "CommonActivatableWidget.h"
#include "Slate/SCommonAnimatedSwitcher.h"

UCommonActivatableWidget* UActivatableWidgetQueueContainer::AddWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, TFunctionRef<void(UCommonActivatableWidget &)> InstanceInitFunc)
{
    return Super::AddWidget<UCommonActivatableWidget>(WidgetClass, InstanceInitFunc);
}

void UActivatableWidgetQueueContainer::RemoveWidget(UCommonActivatableWidget &WidgetToRemove)
{
    Super::RemoveWidget(WidgetToRemove);
}

void UActivatableWidgetQueueContainer::ClearWidgets()
{
    Super::ClearWidgets();
}

int32 UActivatableWidgetQueueContainer::GetNumWidgets() const
{
    return Super::GetNumWidgets();
}

UCommonActivatableWidget* UActivatableWidgetQueueContainer::GetActiveWidget() const
{
    return Super::GetActiveWidget();
}

void UActivatableWidgetQueueContainer::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if WITH_EDITOR
	if (IsDesignTime() && RootContentWidget && RootContentWidget->GetClass() != RootContentWidgetClass)
	{
		// At design time, account for the possibility of the preview class changing
		if (RootContentWidget->GetCachedWidget())
		{
			MySwitcher->GetChildSlot(0)->DetachWidget();
		}

		RootContentWidget = nullptr;
	}
#endif

	if (!RootContentWidget && RootContentWidgetClass)
	{
		// Establish the root content as the blank 0th slot content
		RootContentWidget = CreateWidget<UCommonActivatableWidget>(this, RootContentWidgetClass);
		MySwitcher->GetChildSlot(0)->AttachWidget(RootContentWidget->TakeWidget());
	}

	if (RootContentWidget)
	{
#if WITH_EDITOR
		if (IsDesignTime())
		{
			SetVisibility(IsVisibleInDesigner() ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
		else
#endif
		{
			SetVisibilityInternal(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}
