
#include "StagePlayerComponent.h"
// include Engine
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Components/PrimitiveComponent.h"
#include "Framework/Application/SlateUser.h"
#include "Widgets/SViewport.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePlayerComponent)

APlayerController* UStagePlayerComponent::GetOwningPlayer() const
{
	return Cast<APlayerController>(GetOwner());
}

ULocalPlayer* UStagePlayerComponent::GetOwningLocalPlayer() const
{
	return GetOwningPlayer()->GetLocalPlayer();
}

void UStagePlayerComponent::InteractionMouseEvent()
{
	auto PC = GetOwningPlayer();
	auto LocalPlayer = GetOwningLocalPlayer();

	if (!IsValid(PC) || !LocalPlayer)
	{
		return;
	}

	UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient;
	if (!ViewportClient)
	{
		return;
	}

	TSharedPtr<const FSlateUser> SlateUser = LocalPlayer->GetSlateUser();
	bool bIsWidgetDirectlyUnderCurosr = SlateUser && SlateUser->IsWidgetDirectlyUnderCursor(ViewportClient->GetGameViewportWidget());
	if (!bIsWidgetDirectlyUnderCurosr)
	{
		return;
	}

	FVector2D MousePosition;
	FHitResult HitResult;
	bool bHit = false;

	if (!ViewportClient->GetMousePosition(MousePosition))
	{
		return;
	}

	bHit = PC->GetHitResultAtScreenPosition(MousePosition, PC->CurrentClickTraceChannel, true, /*out*/ HitResult);
	if (!bHit)
	{
		return;
	}

	auto HitActor = HitResult.HitObjectHandle.GetCachedActor();

	if (IsValid(HitActor))
	{
		OnInteractionActor(HitActor);
	}
}

void UStagePlayerComponent::SetHealth(int NewHealth)
{
	if (NewHealth < 0) {
		Health = 0;
	}
	else {
		Health = NewHealth;
	}
}

void UStagePlayerComponent::SetUsePoint(int NewUsePoint)
{
	if (NewUsePoint < 0) {
		UsePoint = 0;
	}
	else{
		UsePoint = NewUsePoint;
	}
}
