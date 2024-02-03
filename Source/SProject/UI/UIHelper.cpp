
#include "UIHelper.h"
// include Engine
#include "Engine/GameInstance.h"
#include "CommonActivatableWidget.h"
#include "NativeGameplayTags.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
// include Plugin
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameUIManagerSubsystem.h"
#include "GameUIPolicy.h"
#include "CommonLocalPlayer.h"
#include "PrimaryGameLayout.h"
// include Project
#include "GameplayMessage/WidgetLayerMessage.h"
#include "UI/SubActivatableWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIHelper)

USubActivatableWidget* UUIHelper::PushContentToLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<USubActivatableWidget> WidgetClass)
{
	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (!ensure(WidgetClass))
	{
		return nullptr;
	}

	if (UPrimaryGameLayout* GameLayout = UPrimaryGameLayout::GetPrimaryGameLayout(PlayerController))
	{
		if (UCommonActivatableWidgetContainerBase* Layer = GameLayout->GetLayerWidget(LayerName))
		{
			if (IActivatableWidgetContainerInterface* ILayer = Cast<IActivatableWidgetContainerInterface>(Layer))
			{
				TScriptInterface<IActivatableWidgetContainerInterface> LayerInterface;
				LayerInterface.SetObject(Layer);
				LayerInterface.SetInterface(ILayer);

				USubActivatableWidget* NewWidget = Layer->AddWidget<USubActivatableWidget>(WidgetClass, [LayerName](USubActivatableWidget& NewWidget)
				{
					NewWidget.LayerName = LayerName;
				});
				if (UGameplayMessageSubsystem* Subsystem = PlayerController->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>())
				{
					Subsystem->BroadcastMessage(
						LayerName,
						FWidgetLayerMessage{
							.GameplayTag = LayerName,
							.ParentWidget = nullptr,
							.CurrentStack = Layer->GetNumWidgets(),
							.CurrentWidget = NewWidget
						});
				}
				return NewWidget;
			}
		}
	}

	return nullptr;
}

void UUIHelper::RemoveContentToLayer_ForPlayer(APlayerController* PlayerController, USubActivatableWidget* RemoveToWidget)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (!ensure(RemoveToWidget))
	{
		return;
	}

	if (UPrimaryGameLayout* GameLayout = UPrimaryGameLayout::GetPrimaryGameLayout(PlayerController))
	{
		FGameplayTag LayerName = RemoveToWidget->LayerName;
		if (UCommonActivatableWidgetContainerBase* Layer = GameLayout->GetLayerWidget(LayerName))
		{
			if (IActivatableWidgetContainerInterface* ILayer = Cast<IActivatableWidgetContainerInterface>(Layer))
			{
				TScriptInterface<IActivatableWidgetContainerInterface> LayerInterface;
				LayerInterface.SetObject(Layer);
				LayerInterface.SetInterface(ILayer);

				USubActivatableWidget* PrevWidget = nullptr;
				for (UCommonActivatableWidget* Widget : Layer->GetWidgetList())
				{
					if (Widget == RemoveToWidget)
					{
						break;
					}
					PrevWidget = Cast<USubActivatableWidget>(Widget);
				}

				Layer->RemoveWidget(*RemoveToWidget);

				if (UGameplayMessageSubsystem* Subsystem = PlayerController->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>())
				{
					Subsystem->BroadcastMessage(
						LayerName,
						FWidgetLayerMessage{
							.GameplayTag = LayerName,
							.ParentWidget = nullptr,
							.CurrentStack = Layer->GetNumWidgets(),
							.CurrentWidget = PrevWidget
						});
				}
			}
		}
	}
}

void UUIHelper::PopContentToLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (UPrimaryGameLayout* GameLayout = UPrimaryGameLayout::GetPrimaryGameLayout(PlayerController))
	{
		if (UCommonActivatableWidgetContainerBase* Layer = GameLayout->GetLayerWidget(LayerName))
		{
			if (IActivatableWidgetContainerInterface* ILayer = Cast<IActivatableWidgetContainerInterface>(Layer))
			{
				TScriptInterface<IActivatableWidgetContainerInterface> LayerInterface;
				LayerInterface.SetObject(Layer);
				LayerInterface.SetInterface(ILayer);

				if (USubActivatableWidget* ParentWidget = Cast<USubActivatableWidget>(Layer->GetActiveWidget()))
				{
					USubActivatableWidget* PrevWidget = nullptr;
					for (UCommonActivatableWidget* Widget : Layer->GetWidgetList())
					{
						if (Widget == ParentWidget)
						{
							break;
						}
						PrevWidget = Cast<USubActivatableWidget>(Widget);
					}

					int32 WidgetNum = Layer->GetNumWidgets();
					Layer->RemoveWidget(*ParentWidget);

					if (UGameplayMessageSubsystem* Subsystem = PlayerController->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>())
					{
						Subsystem->BroadcastMessage(
							LayerName,
							FWidgetLayerMessage{
								.GameplayTag = LayerName,
								.ParentWidget = nullptr,
								.CurrentStack = WidgetNum - 1,
								.CurrentWidget = PrevWidget
							});
					}
				}
			}
		}
	}
}

void UUIHelper::PopContentToTopSubLayer_ForPlayer(APlayerController * PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (UPrimaryGameLayout* GameLayout = UPrimaryGameLayout::GetPrimaryGameLayout(PlayerController))
	{
		if (UCommonActivatableWidgetContainerBase* Layer = GameLayout->GetLayerWidget(LayerName))
		{
			if (USubActivatableWidget* ParnetWidget = Cast<USubActivatableWidget>(Layer->GetActiveWidget()))
			{
				if (TScriptInterface<IActivatableWidgetContainerInterface> SubLayer = ParnetWidget->GetSubLayer(SubLayerName))
				{
					if (USubActivatableWidget* ChildWidget = Cast<USubActivatableWidget>(Layer->GetActiveWidget()))
					{
						ParnetWidget->RemoveWidgetToSubLayer(ChildWidget);
					}
				}
			}
		}
	}
}

void UUIHelper::ClearLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTagContainer LayerNames)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (UPrimaryGameLayout* GameLayout = UPrimaryGameLayout::GetPrimaryGameLayout(PlayerController))
	{
		for (const FGameplayTag& LayerName : LayerNames)
		{
			if (UCommonActivatableWidgetContainerBase* Layer = GameLayout->GetLayerWidget(LayerName))
			{
				if (IActivatableWidgetContainerInterface* ILayer = Cast<IActivatableWidgetContainerInterface>(Layer))
				{
					TScriptInterface<IActivatableWidgetContainerInterface> LayerInterface;
					LayerInterface.SetObject(Layer);
					LayerInterface.SetInterface(ILayer);

					Layer->ClearWidgets();
					if (UGameplayMessageSubsystem* Subsystem = PlayerController->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>())
					{
						Subsystem->BroadcastMessage(
							LayerName,
							FWidgetLayerMessage{
								.GameplayTag = LayerName,
								.ParentWidget = nullptr,
								.CurrentStack = 0,
								.CurrentWidget = nullptr
							});
					}
				}
			}
		}
	}
}

void UUIHelper::ClearTopSubLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTagContainer SubLayerNames)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (UPrimaryGameLayout* GameLayout = UPrimaryGameLayout::GetPrimaryGameLayout(PlayerController))
	{
		if (UCommonActivatableWidgetContainerBase* Layer = GameLayout->GetLayerWidget(LayerName))
		{
			if (USubActivatableWidget* ParentWidget = Cast<USubActivatableWidget>(Layer->GetActiveWidget()))
			{
				ParentWidget->ClearSubLayer(SubLayerNames);
			}
		}
	}
}

FText UUIHelper::FormatWithSuffix(int64 Value, int32 FractionalDigits)
{
	static const FString Suffixes[] = { TEXT(""), TEXT("K"), TEXT("M"), TEXT("G"), TEXT("T"), TEXT("P") };
	static const int32 NumSuffixes = sizeof(Suffixes) / sizeof(*Suffixes);

	static FNumberFormattingOptions NumberFormattingOptions =
		FNumberFormattingOptions()
		.SetAlwaysSign(false)
		.SetUseGrouping(true)
		.SetRoundingMode(ERoundingMode::HalfToEven)
		.SetMinimumIntegralDigits(0)
		.SetMaximumIntegralDigits(TNumericLimits<int32>::Max())
		.SetMaximumFractionalDigits(TNumericLimits<int32>::Max());

	int32 SuffixIndex = 0;
	double DoubleValue = FMath::Abs(static_cast<double>(Value));

	while (DoubleValue >= 1000.0 && SuffixIndex < NumSuffixes - 1)
	{
		DoubleValue /= 1000.0;
		++SuffixIndex;
	}

	if (Value < 0)
	{
		DoubleValue *= 1.0;
	}

	if (FMath::Abs(DoubleValue) >= 100.0)
	{
		NumberFormattingOptions.SetMinimumFractionalDigits(FMath::Max(0, FractionalDigits - 2));
	}
	else if (FMath::Abs(DoubleValue) >= 10.0)
	{
		NumberFormattingOptions.SetMinimumFractionalDigits(FMath::Max(0, FractionalDigits - 1));
	}
	else
	{
		NumberFormattingOptions.SetMinimumFractionalDigits(FMath::Max(0, FractionalDigits));
	}

	return FText::Format(FTextFormat::FromString(TEXT("{0}{1}")), FText::AsNumber(DoubleValue, &NumberFormattingOptions), FText::FromString(Suffixes[SuffixIndex]));
}
