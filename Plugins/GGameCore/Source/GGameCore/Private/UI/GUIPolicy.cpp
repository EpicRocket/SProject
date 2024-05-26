
#include "UI/GUIPolicy.h"
#include "GameFramework/PlayerController.h"
#include "Core/GLocalPlayer.h"
#include "UI/PrimaryGameLayout.h"

void UGUIPolicy::NotifyPlayerAdded(UGLocalPlayer* LocalPlayer)
{
	check(LocalPlayer);
	LocalPlayer->OnPlayerControllerSet.AddWeakLambda(this, [this](UGLocalPlayer* LocalPlayer, AGPlayerController* PlayerController)
	{
		NotifyPlayerRemoved(LocalPlayer);

		if (FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
		{
			AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
			LayoutInfo->bAddedToViewport = true;
		}
		else
		{
			CreateLayoutWidget(LocalPlayer);
		}
	});

	if (FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
	{
		AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = true;
	}
	else
	{
		CreateLayoutWidget(LocalPlayer);
	}
}

void UGUIPolicy::NotifyPlayerRemoved(UGLocalPlayer* LocalPlayer)
{
}

void UGUIPolicy::NotifyPlayerDestroyed(UGLocalPlayer* LocalPlayer)
{
}

void UGUIPolicy::AddLayoutToViewport(UGLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
	UE_LOG(LogTemp, Log, TEXT("[%s]가 [%s] 플레이어의 루트 레이아웃 [%s]를 뷰포트에 추가하는 중입니다."), *GetName(), *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

}

void UGUIPolicy::CreateLayoutWidget(UGLocalPlayer* LocalPlayer)
{
	if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()); IsValid(PlayerController))
	{
		TSubclassOf<UPrimaryGameLayout> LayoutWidgetClass = LayoutClass.LoadSynchronous();
		if (ensure(LayoutWidgetClass && !LayoutWidgetClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			UPrimaryGameLayout* NewLayoutObject = CreateWidget<UPrimaryGameLayout>(PlayerController, LayoutWidgetClass);
			RootViewportLayouts.Emplace(FRootViewportLayoutInfo{ .LocalPlayer = LocalPlayer, .RootLayout = NewLayoutObject, .bAddedToViewport = true });
			AddLayoutToViewport(LocalPlayer, NewLayoutObject);
		}
	}
}
