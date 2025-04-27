
#include "PlayerInputComponent.h"
// include Engine
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "Engine/HitResult.h"
#include "Framework/Application/SlateUser.h"
#include "Widgets/SViewport.h"
#include "Math/UnitConversion.h"
#include "HAL/PlatformApplicationMisc.h"
#include "EnhancedInputSubsystems.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerInputComponent)

void UPlayerInputComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer()))
	{
		Subsystem->ControlMappingsRebuiltDelegate.AddDynamic(this, &UPlayerInputComponent::OnControlMappingsRebuilt);
	}
}

void UPlayerInputComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
	if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer()))
	{
		Subsystem->ControlMappingsRebuiltDelegate.RemoveDynamic(this, &UPlayerInputComponent::OnControlMappingsRebuilt);
	}
}

FVector2D UPlayerInputComponent::GetFirstInputPosition() const
{
	return FirstInputPosition;
}

FVector2D UPlayerInputComponent::GetLastInputPosition() const
{
	return LastInputPosition;
}

void UPlayerInputComponent::OnControlMappingsRebuilt()
{
	auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	TSortedMap<int32, EPlayerInputType> Inputs;
	for (auto& [Type, IMC] : GetControlMappings())
	{
		if (!IMC)
		{
			continue;
		}

		int32 Priority = 0;
		if (!Subsystem->HasMappingContext(IMC, Priority))
		{
			continue;
		}

		Inputs.Emplace(Priority, Type);
	}
	
	if (Inputs.IsEmpty())
	{
		InputMode = EPlayerInputType::None;
		return;
	}

	for (auto& [_, Type] : Inputs)
	{
		InputMode = Type;
		break;
	}
}

void UPlayerInputComponent::OnPress()
{
	switch (InputMode)
	{
	case EPlayerInputType::Mouse:
		OnMousePress();
		break;

	default:
		FirstInputPosition = FVector2D::ZeroVector;
		LastInputPosition = FVector2D::ZeroVector;
		break;
	}
}

void UPlayerInputComponent::OnDrag()
{
	switch (InputMode)
	{
	case EPlayerInputType::Mouse:
		OnMouseDrag();
		break;

	default:
		FirstInputPosition = FVector2D::ZeroVector;
		LastInputPosition = FVector2D::ZeroVector;
		break;
	}
}

void UPlayerInputComponent::OnRelease()
{
	switch (InputMode)
	{
	case EPlayerInputType::Mouse:
		OnMouseRelease();
		break;

	default:
		FirstInputPosition = FVector2D::ZeroVector;
		LastInputPosition = FVector2D::ZeroVector;
		break;
	}
}

void UPlayerInputComponent::OnMousePress()
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
	UE_LOGFMT(
		LogTemp, Log, "Mouse Pressed: {Pos}", bMousePressed ? TEXT("true") : TEXT("false")
	);
}

void UPlayerInputComponent::OnMouseDrag()
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

	if (!ViewportClient->GetMousePosition(LastInputPosition))
	{
		return;
	}

	auto Dist = (FirstInputPosition - LastInputPosition).SizeSquared();
	float DragTriggerDistance;

	const float DragTriggerDistanceInInches = FUnitConversion::Convert(1.0f, EUnit::Millimeters, EUnit::Inches);
	FPlatformApplicationMisc::ConvertInchesToPixels(DragTriggerDistanceInInches, DragTriggerDistance);
	DragTriggerDistance = FMath::Max(DragTriggerDistance, 5.0F);

	if (Dist >= DragTriggerDistance)
	{
		bMousePressed = false;
	}
}

void UPlayerInputComponent::OnMouseRelease()
{
	if (bMousePressed)
	{
		auto PC = GetOwningPlayer();
		auto LocalPlayer = GetOwningLocalPlayer();
		if (!IsValid(PC) || !IsValid(LocalPlayer))
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
	bMousePressed = false;
}
