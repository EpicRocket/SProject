

#include "AsyncAction_PushContentToSubLayer.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "PrimaryGameLayout.h"
#include "UObject/Stack.h"
#include "GameUIManagerSubsystem.h"
#include "GameUIPolicy.h"
#include "CommonLocalPlayer.h"
#include "CommonUIExtensions.h"
// include Project
#include "UI/UIHelper.h"
#include "UI/CustomActivatableWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_PushContentToSubLayer)

UAsyncAction_PushContentToSubLayer* UAsyncAction_PushContentToSubLayer::PushContentToSubLayer(UCustomActivatableWidget* ParentWidget, UPARAM(meta = (AllowAbstract = false)) TSoftClassPtr<UCustomActivatableWidget> InWidgetClass, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, bool bInSuspendInputUntilComplete)
{
	if (!IsValid(ParentWidget) || InWidgetClass.IsNull())
	{
		return nullptr;
	}

	if (UWorld* World = GEngine->GetWorldFromContextObject(ParentWidget, EGetWorldErrorMode::LogAndReturnNull))
	{
		UAsyncAction_PushContentToSubLayer* Action = NewObject<UAsyncAction_PushContentToSubLayer>();
		Action->LayerName = InSubLayerName;
		Action->bSuspendInputUntilComplete = bInSuspendInputUntilComplete;
		Action->ParentWidgetPtr = ParentWidget;
		Action->WidgetClass = InWidgetClass;
		Action->RegisterWithGameInstance(ParentWidget->GetWorld());

		return Action;
	}

	return nullptr;
}

UAsyncAction_PushContentToSubLayer* UAsyncAction_PushContentToSubLayer::PushContentToTopSubLayer(APlayerController* InOwningPlayer, UPARAM(meta = (AllowAbstract = false)) TSoftClassPtr<UCustomActivatableWidget> InWidgetClass, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag InLayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, bool bInSuspendInputUntilComplete)
{
	if (!IsValid(InOwningPlayer) || InWidgetClass.IsNull())
	{
		return nullptr;
	}

	if (UWorld* World = GEngine->GetWorldFromContextObject(InOwningPlayer, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UPrimaryGameLayout* GameLayout = UPrimaryGameLayout::GetPrimaryGameLayout(InOwningPlayer))
		{
			if (UCommonActivatableWidgetContainerBase* Layer = GameLayout->GetLayerWidget(InLayerName))
			{
				UAsyncAction_PushContentToSubLayer* Action = NewObject<UAsyncAction_PushContentToSubLayer>();
				Action->LayerName = InSubLayerName;
				Action->bSuspendInputUntilComplete = bInSuspendInputUntilComplete;
				Action->ParentWidgetPtr = Cast<UCustomActivatableWidget>(Layer->GetActiveWidget());
				Action->WidgetClass = InWidgetClass;
				Action->RegisterWithGameInstance(InOwningPlayer->GetWorld());

				return Action;
			}
		}
	}

	return nullptr;
}

void UAsyncAction_PushContentToSubLayer::Cancel()
{
	Super::Cancel();

	if (StreamingHandle.IsValid())
	{
		StreamingHandle->CancelHandle();
		StreamingHandle.Reset();
	}
}

void UAsyncAction_PushContentToSubLayer::Activate()
{
	if (UCustomActivatableWidget* ParentWidget = ParentWidgetPtr.Get())
	{
		static FName NAME_PushingWidgetToSubLayer("PushingWidgetToSubLayer");
		const FName SuspendInputToken = bSuspendInputUntilComplete ? UCommonUIExtensions::SuspendInputForPlayer(ParentWidget->GetOwningPlayer(), NAME_PushingWidgetToSubLayer) : NAME_None;

		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		StreamingHandle = StreamableManager.RequestAsyncLoad(
			WidgetClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateWeakLambda(
				ParentWidget,
				[this, ThisPtr = TWeakObjectPtr<ThisClass>(this), SuspendInputToken, ParentWidget]()
				{
					UCommonUIExtensions::ResumeInputForPlayer(ParentWidget->GetOwningPlayer(), SuspendInputToken);
					if (ThisPtr.IsValid())
					{
						UCustomActivatableWidget* NewWidget = ParentWidget->PushWidgetToSubLayer(LayerName, WidgetClass.Get(), [this, ThisPtr](UCommonActivatableWidget& Widget)
							{
								if (ThisPtr.IsValid())
								{
									if (UCustomActivatableWidget* NewWidget = Cast<UCustomActivatableWidget>(&Widget))
									{
										NewWidget->LayerName = LayerName;
										BeforePush.Broadcast(NewWidget);
									}
								}
							});

						AfterPush.Broadcast(NewWidget);
					}
					SetReadyToDestroy();
				}
			)
		);

		StreamingHandle->BindCancelDelegate(
			FStreamableDelegate::CreateWeakLambda(
				ParentWidget,
				[this, SuspendInputToken]()
				{
					UCommonUIExtensions::ResumeInputForPlayer(ParentWidgetPtr->GetOwningPlayer(), SuspendInputToken);
					SetReadyToDestroy();
				}
			)
		);
	}
	else
	{
		SetReadyToDestroy();
	}
}
