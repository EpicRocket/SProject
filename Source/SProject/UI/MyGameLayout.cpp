
#include "MyGameLayout.h"
// include Engine
#include "Engine/GameInstance.h"
#include "CommonInputSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
// include GameCore
#include "MyUIPolicy.h"
#include "MyUISubsystem.h"
#include "Core/MyLocalPlayer.h"
#include "Core/MyPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyGameLayout)

int32 InputSuspensionCount = 0;

//////////////////////////////////////////////////////////////////////////
// UMyGameLayout
//////////////////////////////////////////////////////////////////////////

/*static*/ UMyGameLayout* UMyGameLayout::Get(const ULocalPlayer* LocalPlayer)
{
	UMyUISubsystem* UISubsystem = UGameInstance::GetSubsystem<UMyUISubsystem>(LocalPlayer->GetGameInstance());
	if (!UISubsystem)
	{
		return nullptr;
	}
	UGUIPolicy* Policy = UISubsystem->GetPolicy();
	if (!Policy)
	{
		return nullptr;
	}
	return Cast<UMyGameLayout>(Policy->GetRootLayout(Cast<UGLocalPlayer>(LocalPlayer)));
}

void UMyGameLayout::OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning)
{
	if (bIsTransitioning)
	{
		const FName SuspendToken = UMyGameLayoutHelper::SuspendInputForPlayer(GetOwningLocalPlayer(), TEXT("GlobalStackTransion"));
		SuspendInputTokens.Add(SuspendToken);
	}
	else
	{
		if (ensure(SuspendInputTokens.Num() > 0))
		{
			const FName SuspendToken = SuspendInputTokens.Pop();
			UMyGameLayoutHelper::ResumeInputForPlayer(GetOwningLocalPlayer(), SuspendToken);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// UMyGameLayoutHelper
//////////////////////////////////////////////////////////////////////////

FName UMyGameLayoutHelper::SuspendInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendReason)
{
	if (UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer))
	{
		InputSuspensionCount = (InputSuspensionCount >= TNumericLimits<int32>::Max() - 1) ? 0 : InputSuspensionCount + 1;
		FName SuspendToken = SuspendReason;
		SuspendToken.SetNumber(InputSuspensionCount);

		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, true);

		return SuspendToken;
	}

	return NAME_None;
}

void UMyGameLayoutHelper::ResumeInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendToken)
{
	if (SuspendToken == NAME_None)
	{
		return;
	}

	if (UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer))
	{
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, false);
	}
}

UCommonActivatableWidget* UMyGameLayoutHelper::PushContentToLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (!ensure(LayerName.IsValid()))
	{
		return nullptr;
	}

	if (!ensure(WidgetClass != nullptr))
	{
		return nullptr;
	}

	ULocalPlayer const* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!ensure(LocalPlayer))
	{
		return nullptr;
	}

	auto GameLayout = UMyGameLayout::Get(LocalPlayer);
	if (!GameLayout)
	{
		return nullptr;
	}

	auto Layer = GameLayout->GetLayer(LayerName);
	if (!ensure(Layer))
	{
		return nullptr;
	}

	return Layer->AddWidget(WidgetClass);
}

void UMyGameLayoutHelper::RemoveContentFromLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UCommonActivatableWidget* WidgetToRemove)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (!ensure(LayerName.IsValid()))
	{
		return;
	}

	if (!ensure(WidgetToRemove))
	{
		return;
	}

	ULocalPlayer const* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!ensure(LocalPlayer))
	{
		return;
	}

	auto GameLayout = UMyGameLayout::Get(LocalPlayer);
	if (!GameLayout)
	{
		return;
	}

	auto Layer = GameLayout->GetLayer(LayerName);
	if (!ensure(Layer))
	{
		return;
	}

	Layer->RemoveWidget(*WidgetToRemove);
}

void UMyGameLayoutHelper::RemoveContent(const APlayerController* PlayerController, UCommonActivatableWidget* WidgetToRemove)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (!ensure(WidgetToRemove))
	{
		return;
	}

	ULocalPlayer const* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!ensure(LocalPlayer))
	{
		return;
	}

	auto GameLayout = UMyGameLayout::Get(LocalPlayer);
	if (!GameLayout)
	{
		return;
	}

	for (auto Layer : GameLayout->GetLayers())
	{
		Layer->RemoveWidget(*WidgetToRemove);
	}
}

void UMyGameLayoutHelper::ClearContentFromLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	if (!ensure(LayerName.IsValid()))
	{
		return;
	}

	ULocalPlayer const* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!ensure(LocalPlayer))
	{
		return;
	}

	auto GameLayout = UMyGameLayout::Get(LocalPlayer);
	if (!GameLayout)
	{
		return;
	}

	auto Layer = GameLayout->GetLayer(LayerName);
	if (!ensure(Layer))
	{
		return;
	}

	Layer->ClearWidgets();
}

void UMyGameLayoutHelper::ClearContent(const APlayerController* PlayerController)
{
	if (!ensure(PlayerController))
	{
		return;
	}

	ULocalPlayer const* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!ensure(LocalPlayer))
	{
		return;
	}

	auto GameLayout = UMyGameLayout::Get(LocalPlayer);
	if (!GameLayout)
	{
		return;
	}

	for (auto Layer : GameLayout->GetLayers())
	{
		Layer->ClearWidgets();
	}
}
