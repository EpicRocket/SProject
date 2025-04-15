
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
	if (FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
	{
		RemoveLayoutFromViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = false;
	}
}

void UGUIPolicy::NotifyPlayerDestroyed(UGLocalPlayer* LocalPlayer)
{
	NotifyPlayerRemoved(LocalPlayer);
	LocalPlayer->OnPlayerControllerSet.RemoveAll(this);
	const int32 LayoutInfoIdx = RootViewportLayouts.IndexOfByKey(LocalPlayer);
	if (LayoutInfoIdx != INDEX_NONE)
	{
		UPrimaryGameLayout* Layout = RootViewportLayouts[LayoutInfoIdx].RootLayout;
		RootViewportLayouts.RemoveAt(LayoutInfoIdx);

		RemoveLayoutFromViewport(LocalPlayer, Layout);
		OnRootLayoutReleased(LocalPlayer, Layout);
	}
}

UPrimaryGameLayout* UGUIPolicy::GetRootLayout(const UGLocalPlayer* LocalPlayer)
{
	const FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer);
	return LayoutInfo ? LayoutInfo->RootLayout : nullptr;
}

void UGUIPolicy::AddLayoutToViewport(UGLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
	UE_LOG(LogTemp, Log, TEXT("[%s]가 [%s] 플레이어의 루트 레이아웃 [%s]를 뷰포트에 추가하는 중입니다."), *GetName(), *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));
	
	Layout->SetPlayerContext(FLocalPlayerContext(LocalPlayer));
	Layout->AddToPlayerScreen(1000);

	OnRootLayoutAddedToViewport(LocalPlayer, Layout);
}

void UGUIPolicy::RemoveLayoutFromViewport(UGLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
	TWeakPtr<SWidget> LayoutSlateWidget = Layout->GetCachedWidget();
	if (LayoutSlateWidget.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[%s]가 뷰포트에서 [%s] 플레이어의 루트 레이아웃 [%s]를 제거하고 있습니다."), *GetName(), *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

		Layout->RemoveFromParent();
		if (LayoutSlateWidget.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("플레이어 [%s]의 루트 레이아웃 [%s]가 뷰포트에서 제거되었지만 기본 슬레이트 위젯에 대한 다른 참조는 여전히 존재합니다"), *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));
		}
		OnRootLayoutRemovedFromViewport(LocalPlayer, Layout);
	}
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

void UGUIPolicy::OnRootLayoutAddedToViewport(UGLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
#if WITH_EDITOR
	if (GIsEditor && LocalPlayer->IsPrimaryPlayer())
	{
		FSlateApplication::Get().SetUserFocusToGameViewport(0);
	}
#endif
}

void UGUIPolicy::OnRootLayoutRemovedFromViewport(UGLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
}

void UGUIPolicy::OnRootLayoutReleased(UGLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
}
