

#include "StackActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SSpacer.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StackActivatableWidgetContainer)

namespace StackActivatableWidgetContainer
{
	UCommonActivatableWidget* ActivatableWidgetFromSlate(const TSharedPtr<SWidget>& SlateWidget)
	{
		if (SlateWidget && SlateWidget != SNullWidget::NullWidget && ensure(SlateWidget->GetType().IsEqual(TEXT("SObjectWidget"))))
		{
			UCommonActivatableWidget* ActivatableWidget = Cast<UCommonActivatableWidget>(StaticCastSharedPtr<SObjectWidget>(SlateWidget)->GetWidgetObject());
			if (ensure(ActivatableWidget))
			{
				return ActivatableWidget;
			}
		}
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////
// UStackActivatableWidgetContainer
//////////////////////////////////////////////////////////////////////////

using namespace StackActivatableWidgetContainer;

UStackActivatableWidgetContainer::UStackActivatableWidgetContainer()
	: Super(FObjectInitializer::Get())
	, GeneratedWidgetsPool(*this)
{
	SetVisibilityInternal(ESlateVisibility::SelfHitTestInvisible);
}

UCommonActivatableWidget* UStackActivatableWidgetContainer::AddWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InstanceInitFunc)
{
	if (WidgetClass && WidgetClass->IsChildOf<UCommonActivatableWidget>())
	{
		return AddWidgetInternal(WidgetClass, InstanceInitFunc);
	}
	return nullptr;
}

void UStackActivatableWidgetContainer::RemoveWidget(UCommonActivatableWidget& WidgetToRemove)
{
	if (ensure(WidgetList.Contains(&WidgetToRemove)))
	{
		ReleaseWidget(WidgetToRemove.TakeWidget());
	}
}

void UStackActivatableWidgetContainer::ClearWidgets()
{
	FChildren* StackOverlayChildren = MyStackOverlay->GetAllChildren();

	bool bRemoved = false;
	while (StackOverlayChildren->Num() > 0)
	{
		TSharedPtr<SWidget> WidgetToRelease = StackOverlayChildren->GetChildAt(StackOverlayChildren->Num() - 1);
		if (ensure(WidgetToRelease))
		{
			bRemoved = true;
			ReleaseWidget(WidgetToRelease.ToSharedRef());
		}
	}

	if (!bRemoved)
	{
		return;
	}

	if (UWorld* MyWorld = GetWorld())
	{
		FTimerManager& TimerManager = MyWorld->GetTimerManager();
		TimerManager.SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
			{ InvalidateLayoutAndVolatility(); }));
	}
}


int32 UStackActivatableWidgetContainer::GetNumWidgets() const
{
	return WidgetList.Num();
}


UCommonActivatableWidget* UStackActivatableWidgetContainer::GetActiveWidget() const
{
	if (MyStackOverlay)
	{
		FChildren* StackOverlayChildren = MyStackOverlay->GetAllChildren();
		if (TSharedPtr<SWidget> LastWidgetPtr = StackOverlayChildren->GetChildAt(StackOverlayChildren->Num() - 1))
		{
			return ActivatableWidgetFromSlate(LastWidgetPtr);
		}
	}
	return nullptr;
}

TSharedRef<SWidget> UStackActivatableWidgetContainer::RebuildWidget()
{
	MyOverlay =
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SAssignNew(MyStackOverlay, SOverlay)
		]
		;

	return MyOverlay.ToSharedRef();
}

void UStackActivatableWidgetContainer::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyOverlay.Reset();
	MyStackOverlay.Reset();
	ReleasedWidgets.Empty();
	GeneratedWidgetsPool.ReleaseAllSlateResources();
}

void UStackActivatableWidgetContainer::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();
	if (!IsDesignTime())
	{
		ClearWidgets();
	}
}

void UStackActivatableWidgetContainer::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if WITH_EDITOR
	if (IsDesignTime() && RootContentWidget && RootContentWidget->GetClass() != RootContentWidgetClass)
	{
		RootContentWidget = nullptr;
	}
#endif

	if (!RootContentWidget && RootContentWidgetClass)
	{
		RootContentWidget = CreateWidget<UCommonActivatableWidget>(this, RootContentWidgetClass);
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

void UStackActivatableWidgetContainer::OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget)
{
	if (MyStackOverlay)
	{
		MyStackOverlay->AddSlot()
			[
				AddedWidget.TakeWidget()
			]
			;
	}
}

UCommonActivatableWidget* UStackActivatableWidgetContainer::AddWidgetInternal(TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InitFunc)
{
	if (UCommonActivatableWidget* WidgetInstance = GeneratedWidgetsPool.GetOrCreateInstance(ActivatableWidgetClass))
	{
		InitFunc(*WidgetInstance);
		RegisterInstanceInternal(*WidgetInstance);
		return WidgetInstance;
	}
	return nullptr;
}

void UStackActivatableWidgetContainer::RegisterInstanceInternal(UCommonActivatableWidget& NewWidget)
{
	if (ensure(!WidgetList.Contains(&NewWidget)))
	{
		WidgetList.Add(&NewWidget);
		OnWidgetAddedToList(NewWidget);
		NewWidget.ActivateWidget();
	}
}

void UStackActivatableWidgetContainer::ReleaseWidget(const TSharedRef<SWidget>& WidgetToRelease)
{
	if (UCommonActivatableWidget* ActivatableWidget = ActivatableWidgetFromSlate(WidgetToRelease))
	{
		ActivatableWidget->DeactivateWidget();
		GeneratedWidgetsPool.Release(ActivatableWidget, true);
		WidgetList.Remove(ActivatableWidget);
	}

	if (MyStackOverlay->RemoveSlot(WidgetToRelease))
	{
		ReleasedWidgets.Add(WidgetToRelease);
		if (ReleasedWidgets.Num() == 1)
		{
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this,
																				   [this](float)
																				   {
																					   QUICK_SCOPE_CYCLE_COUNTER(STAT_UStackActivatableWidgetContainer_ReleaseWidget);
																					   ReleasedWidgets.Reset();
																					   return false;
																				   }));
		}
	}
}
