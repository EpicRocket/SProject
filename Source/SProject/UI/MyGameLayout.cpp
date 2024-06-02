
#include "MyGameLayout.h"

#include "CommonInputSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include "MyUIPolicy.h"
#include "MyUISubsystem.h"
#include "GameFramework/MyLocalPlayer.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(MyGameLayout)

int32 InputSuspensionCount = 0;

UMyGameLayout* UMyGameLayout::Get(const ULocalPlayer* LocalPlayer)
{
	UMyUISubsystem* UISubsystem = UMyUISubsystem::Get(LocalPlayer);
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

UCommonActivatableWidget* UMyGameLayoutHelper::BP_PushContentToLayer(const ULocalPlayer* LocalPlayer, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(LocalPlayer) || !ensure(WidgetClass != nullptr))
	{
		return nullptr;
	}

	auto GameLayout = UMyGameLayout::Get(LocalPlayer);
	if (!ensure(GameLayout))
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
