
#include "UIHelper.h"
// include Engine
#include "Engine/GameInstance.h"
#include "CommonActivatableWidget.h"
#include "CommonLocalPlayer.h"
#include "NativeGameplayTags.h"
#include "GameUIPolicy.h"
#include "PrimaryGameLayout.h"
#include "GameUIManagerSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
// include Project
#include "UI/Interface/SubLayerContainerInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIHelper)

UCommonActivatableWidgetContainerBase* UUIHelper::GetLayerContainer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (const UGameUIManagerSubsystem* UIManager = PlayerController->GetGameInstance()->GetSubsystem<UGameUIManagerSubsystem>())
	{
		if (const UGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy())
		{
			if (UPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CastChecked<UCommonLocalPlayer>(PlayerController->GetLocalPlayer())))
			{
				return RootLayout->GetLayerWidget(LayerName);
			}
		}
	}
	return nullptr;
}

UCommonActivatableWidgetContainerBase* UUIHelper::GetSubLayerContainer(const APlayerController* PlayerController, FGameplayTag LayerName, FGameplayTag SubLayerName)
{
	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (UCommonActivatableWidgetContainerBase* LayerContainer = UUIHelper::GetLayerContainer(PlayerController, LayerName))
	{
		if (UCommonActivatableWidget* ActivatableWidget = LayerContainer->GetActiveWidget())
		{
			if (ActivatableWidget->Implements<USubLayerContainerInterface>())
			{
				return Cast<ISubLayerContainerInterface>(ActivatableWidget)->Execute_GetLayerSubContainer(ActivatableWidget, SubLayerName);
			}
		}
	}

	return nullptr;
}

UCommonActivatableWidget* UUIHelper::GetContentFromLayer(const APlayerController* PlayerController, FGameplayTag LayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetLayerContainer(PlayerController, LayerName))
	{
		for (UCommonActivatableWidget* Widget : Container->GetWidgetList())
		{
			if (IsValid(Widget) && Widget->IsA(WidgetClass))
			{
				return Widget;
			}
		}
	}

	return nullptr;
}

UCommonActivatableWidget* UUIHelper::GetContentFromSubLayer(const APlayerController* PlayerController, FGameplayTag LayerName, FGameplayTag SubLayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetSubLayerContainer(PlayerController, LayerName, SubLayerName))
	{
		for (UCommonActivatableWidget* Widget : Container->GetWidgetList())
		{
			if (IsValid(Widget) && Widget->IsA(WidgetClass))
			{
				return Widget;
			}
		}
	}

	return nullptr;
}

UCommonActivatableWidget* UUIHelper::GetActiveContentFromLayer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetLayerContainer(PlayerController, LayerName))
	{
		return Container->GetActiveWidget();
	}

	return nullptr;
}

UCommonActivatableWidget* UUIHelper::GetActiveContentFromSubLayer(const APlayerController* PlayerController, FGameplayTag LayerName, FGameplayTag SubLayerName)
{
	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetSubLayerContainer(PlayerController, LayerName, SubLayerName))
	{
		return Container->GetActiveWidget();
	}

	return nullptr;
}

bool UUIHelper::ExistContentFromLayer(const APlayerController* PlayerController, FGameplayTag LayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(PlayerController))
	{
		return false;
	}

	return IsValid(UUIHelper::GetContentFromLayer(PlayerController, LayerName, WidgetClass));
}

bool UUIHelper::ExistContentFromSubLayer(const APlayerController* PlayerController, FGameplayTag LayerName, FGameplayTag SubLayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(PlayerController))
	{
		return false;
	}

	return IsValid(UUIHelper::GetContentFromSubLayer(PlayerController, LayerName, SubLayerName, WidgetClass));
}

UCommonActivatableWidget* UUIHelper::PushContentToSubLayer_ForPlayer(const APlayerController* PlayerController, FGameplayTag LayerName, FGameplayTag SubLayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetSubLayerContainer(PlayerController, LayerName, SubLayerName))
	{
		return Container->AddWidget(WidgetClass);
	}

	return nullptr;
}

void UUIHelper::PopContentFromLayer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetLayerContainer(PlayerController, LayerName))
	{
		if (Container->GetNumWidgets() > 0)
		{
			if (UCommonActivatableWidget* ActiveWidget = Container->GetActiveWidget())
			{
				Container->RemoveWidget(*ActiveWidget);
			}
		}
	}
}

void UUIHelper::PopContentFromSubLayer(const APlayerController* PlayerController, FGameplayTag LayerName, FGameplayTag SubLayerName)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetSubLayerContainer(PlayerController, LayerName, SubLayerName))
	{
		if (Container->GetNumWidgets() > 0)
		{
			if (UCommonActivatableWidget* ActiveWidget = Container->GetActiveWidget())
			{
				Container->RemoveWidget(*ActiveWidget);
			}
		}
	}
}

void UUIHelper::ClearContentFromLayer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetLayerContainer(PlayerController, LayerName))
	{
		Container->ClearWidgets();
	}
}

void UUIHelper::ClearContentFromSubLayer(const APlayerController* PlayerController, FGameplayTag LayerName, FGameplayTag SubLayerName)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (UCommonActivatableWidgetContainerBase* Container = UUIHelper::GetSubLayerContainer(PlayerController, LayerName, SubLayerName))
	{
		Container->ClearWidgets();
	}
}
