

#include "AsyncAction_PushContentToLayer.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "UObject/Stack.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
// include Plugin
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameUIManagerSubsystem.h"
#include "CommonUIExtensions.h"
#include "CommonLocalPlayer.h"
#include "GameUIPolicy.h"
#include "PrimaryGameLayout.h"
// include Project
#include "UI/SubActivatableWidget.h"
#include "UI/GameplayMessage/WidgetLayerMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_PushContentToLayer)

UAsyncAction_PushContentToLayer* UAsyncAction_PushContentToLayer::PushContentToLayer(APlayerController* InOwningPlayer, UPARAM(meta = (AllowAbstract = false)) TSoftClassPtr<USubActivatableWidget> InWidgetClass, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag InLayerName, bool bInSuspendInputUntilComplete)
{
	if (InWidgetClass.IsNull())
	{
		return nullptr;
	}

	if (UWorld* World = GEngine->GetWorldFromContextObject(InOwningPlayer, EGetWorldErrorMode::LogAndReturnNull))
	{
		UAsyncAction_PushContentToLayer* Action = NewObject<UAsyncAction_PushContentToLayer>();
		Action->WidgetClass = InWidgetClass;
		Action->OwningPlayerPtr = InOwningPlayer;
		Action->LayerName = InLayerName;
		Action->bSuspendInputUntilComplete = bInSuspendInputUntilComplete;
		Action->RegisterWithGameInstance(World);

		return Action;
	}

	return nullptr;
}

void UAsyncAction_PushContentToLayer::Cancel()
{
	Super::Cancel();

	if (StreamingHandle.IsValid())
	{
		StreamingHandle->CancelHandle();
		StreamingHandle.Reset();
	}
}

void UAsyncAction_PushContentToLayer::Activate()
{
	if (UPrimaryGameLayout* GameLayout = UPrimaryGameLayout::GetPrimaryGameLayout(OwningPlayerPtr.Get()))
	{
		static FName NAME_PushingWidgetToSubLayer("PushingWidgetToLayer");
		const FName SuspendInputToken = bSuspendInputUntilComplete ? UCommonUIExtensions::SuspendInputForPlayer(OwningPlayerPtr.Get(), NAME_PushingWidgetToSubLayer) : NAME_None;

		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		StreamingHandle = StreamableManager.RequestAsyncLoad(
			WidgetClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateWeakLambda(
				GameLayout,
				[this, ThisPtr = TWeakObjectPtr<ThisClass>(this), SuspendInputToken, GameLayout]()
				{
					UCommonUIExtensions::ResumeInputForPlayer(OwningPlayerPtr.Get(), SuspendInputToken);
					if (ThisPtr.IsValid())
					{
						if (UCommonActivatableWidgetContainerBase* Layer = GameLayout->GetLayerWidget(LayerName))
						{
							if (IActivatableWidgetContainerInterface* ILayer = Cast<IActivatableWidgetContainerInterface>(Layer))
							{
								TScriptInterface<IActivatableWidgetContainerInterface> LayerInterface;
								LayerInterface.SetObject(Layer);
								LayerInterface.SetInterface(ILayer);

								USubActivatableWidget* NewWidget = Layer->AddWidget<USubActivatableWidget>(
									WidgetClass.Get(),
									[this, ThisPtr](USubActivatableWidget& Widget)
									{
										if (ThisPtr.IsValid())
										{
											Widget.LayerName = LayerName;
											BeforePush.Broadcast(&Widget);
										}
									});

								if (UGameplayMessageSubsystem* Subsystem = OwningPlayerPtr->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>())
								{
									Subsystem->BroadcastMessage(
										LayerName,
										FWidgetLayerMessage{
											.GameplayTag = LayerName,
											.ParentWidget = nullptr,
											.CurrentStack = Layer->GetNumWidgets(),
											.CurrentWidget = NewWidget});
								}

								AfterPush.Broadcast(NewWidget);
							}
						}
					}
					SetReadyToDestroy();
				}));

		StreamingHandle->BindCancelDelegate(
			FStreamableDelegate::CreateWeakLambda(
				GameLayout,
				[this, SuspendInputToken]()
				{
					UCommonUIExtensions::ResumeInputForPlayer(OwningPlayerPtr.Get(), SuspendInputToken);
					SetReadyToDestroy();
				}));
	}
	else
	{
		SetReadyToDestroy();
	}
}
