
#include "PlayerInputComponent.h"
// include Engine
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "Engine/HitResult.h"
#include "Framework/Application/SlateUser.h"
#include "Widgets/SViewport.h"
#include "Math/UnitConversion.h"
#include "HAL/PlatformApplicationMisc.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerInputComponent)

void UPlayerInputComponent::InteractionMouseEvent()
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

void UPlayerInputComponent::OnMousePressed()
{
	auto LocalPlayer = GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient;
	if (!ViewportClient)
	{
		return;
	}

	bMousePressed = ViewportClient->GetMousePosition(FirstInputPosition);
}

void UPlayerInputComponent::OnMouseMoved()
{
	auto LocalPlayer = GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient;
	if (!ViewportClient)
	{
		return;
	}

	FVector2D MousePosition;
	if (!ViewportClient->GetMousePosition(MousePosition))
	{
		return;
	}

	auto Dist = (FirstInputPosition - MousePosition).SizeSquared();
	float DragTriggerDistance;

	const float DragTriggerDistanceInInches = FUnitConversion::Convert(1.0f, EUnit::Millimeters, EUnit::Inches);
	FPlatformApplicationMisc::ConvertInchesToPixels(DragTriggerDistanceInInches, DragTriggerDistance);
	DragTriggerDistance = FMath::Max(DragTriggerDistance, 5.0F);

	if (Dist >= DragTriggerDistance)
	{
		bMousePressed = false;
	}
}

void UPlayerInputComponent::OnMouseReleased()
{
	if (bMousePressed)
	{
		InteractionMouseEvent();
	}
	bMousePressed = false;
}

FVector2D UPlayerInputComponent::GetFirstInputPosition() const
{
	return FirstInputPosition;
}

FVector2D UPlayerInputComponent::GetLastInputPosition() const
{
	auto LocalPlayer = GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		return FirstInputPosition;
	}

	UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient;
	if (!ViewportClient)
	{
		return FirstInputPosition;
	}

	if (!ViewportClient->GetMousePosition(LastInputPosition))
	{
		return FirstInputPosition;
	}

	return LastInputPosition;
}
