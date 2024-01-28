

#include "CustomActivatableWidget.h"
// include Plugin
#include "Engine/GameInstance.h"
// include Plugin
#include "PrimaryGameLayout.h"
#include "GameFramework/GameplayMessageSubsystem.h"
// include Project
#include "UIHelper.h"
#include "GameplayMessage/WidgetLayerMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomActivatableWidget)

UCustomActivatableWidget::UCustomActivatableWidget()
	: Super(FObjectInitializer::Get())
{
#if WITH_EDITOR
	PaletteCategory = FText::FromString(TEXT("Project Only"));
#endif
}

TScriptInterface<IActivatableWidgetContainerInterface> UCustomActivatableWidget::GetSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName) const
{
	return SubLayers.Contains(InSubLayerName) ? SubLayers[InSubLayerName] : TScriptInterface<IActivatableWidgetContainerInterface>{};
}

UCustomActivatableWidget* UCustomActivatableWidget::GetActiveWidget(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName) const
{
	if (SubLayers.Contains(InSubLayerName))
	{
		return Cast<UCustomActivatableWidget>(SubLayers[InSubLayerName]->GetActiveWidget());
	}
	return nullptr;
}

void UCustomActivatableWidget::Close(TFunction<void(bool)> AfterCallback)
{
	OnNativeClose(AfterCallback);
}

void UCustomActivatableWidget::BP_Close()
{
	Close();
}

UCustomActivatableWidget* UCustomActivatableWidget::PushWidgetToSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, TSubclassOf<UCustomActivatableWidget> WidgetClass, TFunction<void(UCommonActivatableWidget&)> InstanceInitFunc)
{
	if (!WidgetClass || !SubLayers.Contains(InSubLayerName))
	{
		return nullptr;
	}

	TScriptInterface<IActivatableWidgetContainerInterface>& SubLayer = SubLayers[InSubLayerName];
	if (SubLayer)
	{
		UCustomActivatableWidget* NewWidget = Cast<UCustomActivatableWidget>(
			SubLayer->AddWidget(
				WidgetClass,
				[this, InSubLayerName, InstanceInitFunc, ThisPtr = TWeakObjectPtr<ThisClass>(this)](UCommonActivatableWidget& NewWidget) {
					if (ThisPtr.IsValid())
					{
						if (InstanceInitFunc)
						{
							InstanceInitFunc(NewWidget);
						}

						if (UCustomActivatableWidget* ChildWidget = Cast<UCustomActivatableWidget>(&NewWidget))
						{
							ChildWidget->LayerName = InSubLayerName;
							ChildWidget->ParentLayerWidget = this;
						}
					}
					
				}));

		if (IsValid(NewWidget))
		{
			WidgetPtrMap.FindOrAdd(InSubLayerName).Emplace(NewWidget);
			if (UGameplayMessageSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>())
			{
				Subsystem->BroadcastMessage(
					InSubLayerName,
					FWidgetLayerMessage{
						.GameplayTag = InSubLayerName,
						.Layer = SubLayer,
						.CurrentStack = SubLayer->GetNumWidgets(),
						.CurrentWidget = NewWidget
					});
			}

			return NewWidget;
		}
	}
	return nullptr;
}

UCustomActivatableWidget* UCustomActivatableWidget::BP_PushWidgetToSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, TSubclassOf<UCustomActivatableWidget> WidgetClass)
{
	return PushWidgetToSubLayer(InSubLayerName, WidgetClass, [](UCommonActivatableWidget& NewWidget){});
}

void UCustomActivatableWidget::RemoveWidgetToSubLayer(UCustomActivatableWidget* RemoveToWidget)
{
	if (!IsValid(RemoveToWidget))
	{
		return;
	}

	if (!SubLayers.Contains(RemoveToWidget->LayerName))
	{
		return;
	}

	TScriptInterface<IActivatableWidgetContainerInterface>& SubLayer = SubLayers[RemoveToWidget->LayerName];
	if (SubLayer)
	{
		TArray<TWeakObjectPtr<UCustomActivatableWidget>>& WidgetPtrs = WidgetPtrMap.FindOrAdd(RemoveToWidget->LayerName);
		
		for (int32 Index = WidgetPtrs.Num() - 1; Index >= 0; --Index)
		{
			UCustomActivatableWidget* PickupWidget = WidgetPtrs[Index].Get();
			if (!IsValid(PickupWidget))
			{
				WidgetPtrs.RemoveAt(Index);
				continue;
			}

			if (PickupWidget == RemoveToWidget)
			{
				FGameplayTag RemoveWidgetLayerName = PickupWidget->LayerName;
				
				WidgetPtrs.RemoveAt(Index);
				SubLayer->RemoveWidget(*RemoveToWidget);

				if (UGameplayMessageSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>())
				{
					Subsystem->BroadcastMessage(
						RemoveWidgetLayerName,
						FWidgetLayerMessage{
							.GameplayTag = RemoveWidgetLayerName,
							.Layer = SubLayer,
							.CurrentStack = WidgetPtrs.Num(),
							.CurrentWidget = WidgetPtrs.IsValidIndex(Index - 1) ? WidgetPtrs[Index - 1].Get() : nullptr
						});
				}
				return;
			}
		}
	}
}

void UCustomActivatableWidget::ClearSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTagContainer InSubLayerNames)
{
	for (const FGameplayTag& SubLayerName : InSubLayerNames)
	{
		if (SubLayers.Contains(SubLayerName))
		{
			TScriptInterface<IActivatableWidgetContainerInterface>& SubLayer = SubLayers[SubLayerName];
			if (SubLayer)
			{
				SubLayer->ClearWidgets();
				WidgetPtrMap.FindOrAdd(SubLayerName).Empty();

				if (UGameplayMessageSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>())
				{
					Subsystem->BroadcastMessage(
						SubLayerName,
						FWidgetLayerMessage{
							.GameplayTag = SubLayerName,
							.Layer = SubLayer,
							.CurrentStack = 0,
							.CurrentWidget = nullptr
						});
				}
			}
		}
	}
}

void UCustomActivatableWidget::Clear()
{
	for (auto& [SubLayerName, SubLayer] : SubLayers)
	{
		if (SubLayer)
		{
			SubLayer->ClearWidgets();
			WidgetPtrMap.FindOrAdd(SubLayerName).Empty();

			if (UGameplayMessageSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>())
			{
				Subsystem->BroadcastMessage(
					SubLayerName,
					FWidgetLayerMessage{
						.GameplayTag = SubLayerName,
						.Layer = SubLayer,
						.CurrentStack = 0,
						.CurrentWidget = nullptr
					});
			}
		}
	}
}

void UCustomActivatableWidget::RequestPopWidgetToSubLayer(FGameplayTag SubLayerName, TFunction<void(TFunction<void(bool)>)>& OutTask)
{
	if (!SubLayers.Contains(SubLayerName))
	{
		return;
	}

	if (TScriptInterface<IActivatableWidgetContainerInterface>& SubLayer = SubLayers[SubLayerName])
	{
		TArray<TWeakObjectPtr<UCustomActivatableWidget>>& WidgetPtrArray = WidgetPtrMap.FindOrAdd(SubLayerName);
		UCustomActivatableWidget* RemoveToWidget = nullptr;
		while (WidgetPtrArray.Num() > 0)
		{
			TWeakObjectPtr<UCustomActivatableWidget>& WidgetPtr = WidgetPtrArray.Last();
			if (!WidgetPtr.IsValid())
			{
				WidgetPtrArray.Pop(false);
				continue;
			}
			RemoveToWidget = WidgetPtr.Get();
			break;
		}

		if (IsValid(RemoveToWidget))
		{
			OutTask = [this, ThisPtr = TWeakObjectPtr<UCustomActivatableWidget>(this), RemoveToWidget](TFunction<void(bool)> Callback)
				{
					RemoveToWidget->Close(Callback);
				};
		}
	}
}

void UCustomActivatableWidget::RequestClearToSubLayer(FGameplayTagContainer InSubLayerNames, TArray<TFunction<void(TFunction<void(bool)>)>>& OutTask)
{
	for (const FGameplayTag& SubLayerName : InSubLayerNames)
	{
		if (!SubLayers.Contains(SubLayerName))
		{
			return;
		}

		if (TScriptInterface<IActivatableWidgetContainerInterface>& SubLayer = SubLayers[SubLayerName])
		{
			TArray<TWeakObjectPtr<UCustomActivatableWidget>>& WidgetPtrArray = WidgetPtrMap.FindOrAdd(SubLayerName);
			for (int32 Index = WidgetPtrArray.Num() - 1; Index >= 0; --Index)
			{
				TWeakObjectPtr<UCustomActivatableWidget>& WidgetPtr = WidgetPtrArray[Index];
				if (!WidgetPtr.IsValid())
				{
					WidgetPtrArray.RemoveAt(Index);
					continue;
				}

				UCustomActivatableWidget* PickupWidget = WidgetPtr.Get();
				OutTask.Emplace([this, ThisPtr = TWeakObjectPtr<UCustomActivatableWidget>(this), PickupWidget](TFunction<void(bool)> Callback)
					{
						PickupWidget->Close(Callback);
					});
			}
		}
	}
}

void UCustomActivatableWidget::OnNativeClose(TFunction<void(bool)> AfterCallback)
{
	if (ParentLayerWidget)
	{
		OnCloseInternal();
	}
	else
	{
		UUIHelper::RemoveContentToLayer_ForPlayer(GetOwningPlayer(), this);
	}

	if (AfterCallback)
	{
		AfterCallback(false);
	}
}

void UCustomActivatableWidget::OnCloseInternal()
{
	for (auto& [SubLayerName, SubLayer] : SubLayers)
	{
		if (SubLayer)
		{
			SubLayer->ClearWidgets();
		}
	}
	WidgetPtrMap.Empty();
	ParentLayerWidget->RemoveWidgetToSubLayer(this);
}

void UCustomActivatableWidget::RegisterSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, TScriptInterface<IActivatableWidgetContainerInterface> ChildContainer)
{
	if (!IsDesignTime())
	{
		SubLayers.Emplace(InSubLayerName, ChildContainer);
	}
}
