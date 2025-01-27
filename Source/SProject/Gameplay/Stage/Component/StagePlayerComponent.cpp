
#include "StagePlayerComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Components/PrimitiveComponent.h"
#include "Framework/Application/SlateUser.h"
#include "Widgets/SViewport.h"
#include "Math/UnitConversion.h"
#include "HAL/PlatformApplicationMisc.h"
// include Project
#include "Core/MyPlayerController.h"
#include "Gameplay/Stage/GameplayMessage/StagePlayerEventMessage.h"
#include "Gameplay/Team/GameplayTeamSubsystem.h"
#include "Gameplay/Team/GameplayUserPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePlayerComponent)

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

void UStagePlayerComponent::OnMousePressed()
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

	bMousePressed = ViewportClient->GetMousePosition(FirstMousePressPosition);
}

void UStagePlayerComponent::OnMouseMoved()
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

	auto Dist = (FirstMousePressPosition - MousePosition).SizeSquared();
	float DragTriggerDistance;

	const float DragTriggerDistanceInInches = FUnitConversion::Convert(1.0f, EUnit::Millimeters, EUnit::Inches);
	FPlatformApplicationMisc::ConvertInchesToPixels(DragTriggerDistanceInInches, DragTriggerDistance);
	DragTriggerDistance = FMath::Max(DragTriggerDistance, 5.0F);

	if (Dist >= DragTriggerDistance)
	{
		bMousePressed = false;
	}
}

void UStagePlayerComponent::OnMouseReleased()
{
	if (bMousePressed)
	{
		InteractionMouseEvent();
	}
	bMousePressed = false;
}

void UStagePlayerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	auto TeamSubsystem = UWorld::GetSubsystem<UGameplayTeamSubsystem>(GetWorld());
	check(TeamSubsystem);

	auto MyPlayerController = Cast<AMyPlayerController>(GetOwningPlayer());
	auto Player = TeamSubsystem->GetPlayer(MyPlayerController->GetGenericTeamId());
	if (!Player)
	{
		return;
	}

	UserPlayer = Cast<UGameplayUserPlayer>(Player);
	if (!UserPlayer.IsValid())
	{
		return;
	}

	UserPlayer->OwningPlayerController = MyPlayerController;
}

void UStagePlayerComponent::SetHealth(int32 NewHealth)
{
	FStagePlayerHealthMessage Message;
	Message.OldValue = Health;
	Health = FMath::Max(0, NewHealth);
	Message.NewValue = Health;
	//UGameplayMessageSubsystem::Get(this).BroadcastMessage(Stage::Tag_Gameplay_Stage_Player_Health_Changed, Message);
}

void UStagePlayerComponent::SetUsePoint(int32 NewUsePoint)
{
	FStagePlayerUsePointMessage Message;
	Message.OldValue = UsePoint;
	UsePoint = FMath::Max(0, NewUsePoint);
	Message.NewValue = UsePoint;
	//UGameplayMessageSubsystem::Get(this).BroadcastMessage(Stage::Tag_Gameplay_Stage_Player_UsePoint_Changed, Message);
}
