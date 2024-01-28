

#pragma once

#include "Blueprint/UserWidgetPool.h"
#include "ActivatableWidgetContainerInterface.h"

#include "StackActivatableWidgetContainer.generated.h"

class UCommonActivatableWidget;
class SOverlay;
class SSpacer;

UCLASS(BlueprintType, NotBlueprintable)
class UStackActivatableWidgetContainer : public UWidget, public IActivatableWidgetContainerInterface
{
	GENERATED_BODY()

public:
	UStackActivatableWidgetContainer();

	// IActivatableWidgetContainerInterface
    virtual UCommonActivatableWidget* AddWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InstanceInitFunc) override;
	virtual void RemoveWidget(UCommonActivatableWidget& WidgetToRemove) override;
	virtual void ClearWidgets() override;
	virtual int32 GetNumWidgets() const override;
	virtual UCommonActivatableWidget* GetActiveWidget() const override;
	// ~IActivatableWidgetContainerInterface

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* AddWidget(TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass)
	{
		return AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, [](UCommonActivatableWidget&) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* AddWidget(TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InstanceInitFunc)
	{
		// Don't actually add the widget if the cast will fail
		if (ActivatableWidgetClass && ActivatableWidgetClass->IsChildOf<ActivatableWidgetT>())
		{
			return Cast<ActivatableWidgetT>(AddWidgetInternal(ActivatableWidgetClass, [&InstanceInitFunc](UCommonActivatableWidget& WidgetInstance)
				{ InstanceInitFunc(*CastChecked<ActivatableWidgetT>(&WidgetInstance)); }));
		}
		return nullptr;
	}

	const TArray<UCommonActivatableWidget*>& GetWidgetList() const { return WidgetList; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void OnWidgetRebuilt() override;

private:
	virtual void SynchronizeProperties() override;
	virtual void OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget);
	UCommonActivatableWidget* AddWidgetInternal(TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InitFunc);
	void RegisterInstanceInternal(UCommonActivatableWidget& NewWidget);

	void ReleaseWidget(const TSharedRef<SWidget>& WidgetToRelease);

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UCommonActivatableWidget>> WidgetList;

	UPROPERTY(Transient)
	FUserWidgetPool GeneratedWidgetsPool;

	TSharedPtr<SOverlay> MyOverlay;
	TSharedPtr<SOverlay> MyStackOverlay;

	TArray<TSharedPtr<SWidget>> ReleasedWidgets;

	UPROPERTY(EditAnywhere, Category = Content, meta = (AllowPrivateAccess = true))
	TSubclassOf<UCommonActivatableWidget> RootContentWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UCommonActivatableWidget> RootContentWidget;
};
