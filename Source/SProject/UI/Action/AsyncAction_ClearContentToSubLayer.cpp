

#include "AsyncAction_ClearContentToSubLayer.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
// include Plugin
#include "PrimaryGameLayout.h"
#include "GameUIPolicy.h"
#include "CommonLocalPlayer.h"
#include "GameUIManagerSubsystem.h"
// include Project
#include "UI/UIHelper.h"
#include "UI/CustomActivatableWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ClearContentToSubLayer)

UAsyncAction_ClearContentToSubLayer* UAsyncAction_ClearContentToSubLayer::ClearContentToSubLayer(UCustomActivatableWidget* ParentWidget, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTagContainer InSubLayerNames)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(ParentWidget, EGetWorldErrorMode::LogAndReturnNull))
	{
		UAsyncAction_ClearContentToSubLayer* Action = NewObject<UAsyncAction_ClearContentToSubLayer>();
		Action->LayerNames = InSubLayerNames;
		Action->ParentWidgetPtr = ParentWidget;
		return Action;

	}
	return nullptr;
}

UAsyncAction_ClearContentToSubLayer* UAsyncAction_ClearContentToSubLayer::ClearContentToTopSubLayer(APlayerController* InOwningPlayer, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag InLayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTagContainer InSubLayerNames)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(InOwningPlayer, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UPrimaryGameLayout* GameLayout = UPrimaryGameLayout::GetPrimaryGameLayout(InOwningPlayer))
		{
			if (UCommonActivatableWidgetContainerBase* Layer = GameLayout->GetLayerWidget(InLayerName))
			{
				UAsyncAction_ClearContentToSubLayer* Action = NewObject<UAsyncAction_ClearContentToSubLayer>();
				Action->LayerNames = InSubLayerNames;
				Action->ParentWidgetPtr = Cast<UCustomActivatableWidget>(Layer->GetActiveWidget());
				return Action;
			}
		}
	}
	return nullptr;
}

void UAsyncAction_ClearContentToSubLayer::Activate()
{
	if (UCustomActivatableWidget* ParentWidget = ParentWidgetPtr.Get())
	{
		ParentWidget->RequestClearToSubLayer(LayerNames, TaskQueue);
		ExecuteInternal();
	}
	else
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_ClearContentToSubLayer::ExecuteInternal()
{
	if (TaskQueue.Num() > 0)
	{
		TFunction<void(TFunction<void(bool)>)> Task = TaskQueue[0];
		TaskQueue.RemoveAt(0);
		Task([this, ThisPtr = TWeakObjectPtr<ThisClass>(this)](bool bCancel)
			{
				if (ThisPtr.IsValid())
				{
					if (bCancel)
					{
						OnCancel.Broadcast();
						SetReadyToDestroy();
					}
					else
					{
						ExecuteInternal();
					}
				}
				else
				{
					SetReadyToDestroy();
				}
			});
	}
	else
	{
		OnComplete.Broadcast();
		SetReadyToDestroy();
	}
}
