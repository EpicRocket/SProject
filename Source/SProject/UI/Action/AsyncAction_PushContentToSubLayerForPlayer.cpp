
#include "AsyncAction_PushContentToSubLayerForPlayer.h"
// include Engine
#include "Engine/Engine.h"
#include "PrimaryGameLayout.h"
#include "UObject/Stack.h"
#include "CommonUIExtensions.h"
// include Project
#include "UI/UIHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_PushContentToSubLayerForPlayer)

void UAsyncAction_PushContentToSubLayerForPlayer::Cancel()
{
	Super::Cancel();

	if (StreamingHandle.IsValid())
	{
		StreamingHandle->CancelHandle();
		StreamingHandle.Reset();
	}
}

UAsyncAction_PushContentToSubLayerForPlayer* UAsyncAction_PushContentToSubLayerForPlayer::PushContentToSubLayer_ForPlayer(APlayerController* OwningPlayer, TSoftClassPtr<UCommonActivatableWidget> WidgetClass, FGameplayTag LayerName, FGameplayTag SubLayerName, bool bSuspendInputUntilComplete)
{
	if (WidgetClass.IsNull())
	{
		FFrame::KismetExecutionMessage(TEXT("PushContentToSubLayerForPlayer에 NULL 위젯클래스가 전달되었습니다."), ELogVerbosity::Error);
		return nullptr;
	}

	if (UWorld* World = GEngine->GetWorldFromContextObject(OwningPlayer, EGetWorldErrorMode::LogAndReturnNull))
	{
		UAsyncAction_PushContentToSubLayerForPlayer* Action = NewObject<UAsyncAction_PushContentToSubLayerForPlayer>();
		Action->WidgetClass = WidgetClass;
		Action->OwningPlayerPtr = OwningPlayer;
		Action->LayerName = LayerName;
		Action->SubLayerName = SubLayerName;
		Action->bSuspendInputUntilComplete = bSuspendInputUntilComplete;
		Action->RegisterWithGameInstance(World);

		return Action;
	}

	return nullptr;
}

void UAsyncAction_PushContentToSubLayerForPlayer::Activate()
{
	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(OwningPlayerPtr.Get()))
	{
		TWeakObjectPtr<UAsyncAction_PushContentToSubLayerForPlayer> WeakThis = this;

		static FName NAME_PushingWidgetToSubLayer("PushingWidgetToSubLayer");
		const FName SuspendInputToken = bSuspendInputUntilComplete ? UCommonUIExtensions::SuspendInputForPlayer(OwningPlayerPtr.Get(), NAME_PushingWidgetToSubLayer) : NAME_None;

		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		StreamingHandle = StreamableManager.RequestAsyncLoad(
			WidgetClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateWeakLambda(
				RootLayout,
				[this, WeakThis, SuspendInputToken]()
				{
					UCommonUIExtensions::ResumeInputForPlayer(OwningPlayerPtr.Get(), SuspendInputToken);
					if (WeakThis.IsValid())
					{
						UCommonActivatableWidget* Widget = nullptr;
						if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetSubLayerContainer(OwningPlayerPtr.Get(), LayerName, SubLayerName))
						{
							Widget = Container->AddWidget<UCommonActivatableWidget>(WidgetClass.Get(), [this, WeakThis](UCommonActivatableWidget& Widget)
							{
								if (WeakThis.IsValid())
								{
									BeforePush.Broadcast(&Widget);
								}
							});
							AfterPush.Broadcast(Widget);
						}
					}
					SetReadyToDestroy();
				}
			)
		);

		StreamingHandle->BindCancelDelegate(
			FStreamableDelegate::CreateWeakLambda(
				RootLayout,
				[this, SuspendInputToken]()
				{
					UCommonUIExtensions::ResumeInputForPlayer(OwningPlayerPtr.Get(), SuspendInputToken);
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
