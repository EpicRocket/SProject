
#include "StagePlayerComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Components/PrimitiveComponent.h"
#include "Framework/Application/SlateUser.h"
#include "Widgets/SViewport.h"
#include "Math/UnitConversion.h"
#include "HAL/PlatformApplicationMisc.h"
// include GameCore
#include "GMessage/GMessage.h"
// include Project
#include "Core/MyPlayerController.h"
#include "Types/StageTypes.h"
#include "User/StageSubsystem.h"
#include "Table/ConstTable.h"
#include "Table/StageTable.h"
#include "Gameplay/Team/GameplayTeamSubsystem.h"
#include "Gameplay/Team/GameplayUserPlayer.h"
#include "Gameplay/Stage/GameplayMessage/StagePlayerEventMessage.h"
#include "Gameplay/Stage/StageTableRepository.h"

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

FGErrorInfo UStagePlayerComponent::SetDefaults()
{
	auto StageSubsystem = UStageSubsystem::Get(GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UStageSubsystem"));
	}

	auto LastStage = StageSubsystem->GetLastStage();

	FStageTableRow StageTableRow;
	if (auto Err = UStageTableHelper::GetStage(LastStage->Level, StageTableRow); !GameCore::IsOK(Err))
	{
		return Err;
	}

	LastStage->Towers.Empty();
	LastStage->Hp = GetDefault<UConstSettings>()->UserHp;
	LastStage->UsePoint = StageTableRow.UsePoint;

	return GameCore::Pass();
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

FGErrorInfo UStagePlayerComponent::NewStart()
{
	if (auto Err = SetDefaults(); !GameCore::IsOK(Err))
	{
		return Err;
	}

	auto GMessageSubsystem = UGameInstance::GetSubsystem<UGMessageSubsystem>(GetGameInstance<UGameInstance>());
	if (!GMessageSubsystem)
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UGMessageSubsystem"));
	}

	FStagePlayerNewStartMessage Message;
	GMessageSubsystem->BroadcastMessage(Stage::Tag_Gameplay_Stage_Player_NewStart, Message);

	return GameCore::Pass();
}

FGErrorInfo UStagePlayerComponent::Restart()
{
	if (auto Err = SetDefaults(); !GameCore::IsOK(Err))
	{
		return Err;
	}

	auto GMessageSubsystem = UGameInstance::GetSubsystem<UGMessageSubsystem>(GetGameInstance<UGameInstance>());
	if (!GMessageSubsystem)
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UGMessageSubsystem"));
	}

	FStagePlayerRestartMessage Message;
	GMessageSubsystem->BroadcastMessage(Stage::Tag_Gameplay_Stage_Player_Restart, Message);

	return GameCore::Pass();
}

void UStagePlayerComponent::SetHp(int32 NewHp)
{
	auto StageSubsystem = UStageSubsystem::Get(GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UStageSubsystem"));
		return;
	}

	auto GMessageSubsystem = UGameInstance::GetSubsystem<UGMessageSubsystem>(GetGameInstance<UGameInstance>());
	if (!GMessageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UGMessageSubsystem"));
		return;
	}

	FStagePlayerHealthMessage Message;
	Message.OldValue = StageSubsystem->GetLastStage()->Hp;
	StageSubsystem->GetLastStage()->Hp = NewHp;
	Message.NewValue = StageSubsystem->GetLastStage()->Hp;
	GMessageSubsystem->BroadcastMessage(Stage::Tag_Gameplay_Stage_Player_Health_Changed, Message);
}

int32 UStagePlayerComponent::GetHp() const
{
	auto StageSubsystem = UStageSubsystem::Get(GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UStageSubsystem"));
		return 0;
	}

	return StageSubsystem->GetLastStage()->Hp;
}

void UStagePlayerComponent::SetUsePoint(int64 NewUsePoint)
{
	auto StageSubsystem = UStageSubsystem::Get(GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UStageSubsystem"));
		return;
	}

	auto GMessageSubsystem = UGameInstance::GetSubsystem<UGMessageSubsystem>(GetGameInstance<UGameInstance>());
	if (!GMessageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UGMessageSubsystem"));
		return;
	}

	FStagePlayerUsePointMessage Message;
	Message.OldValue = StageSubsystem->GetLastStage()->UsePoint;
	StageSubsystem->GetLastStage()->UsePoint = NewUsePoint;
	Message.NewValue = StageSubsystem->GetLastStage()->UsePoint;
	GMessageSubsystem->BroadcastMessage(Stage::Tag_Gameplay_Stage_Player_UsePoint_Changed, Message);
}

int64 UStagePlayerComponent::GetUsePoint() const
{
	auto StageSubsystem = UStageSubsystem::Get(GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UStageSubsystem"));
		return 0;
	}

	return StageSubsystem->GetLastStage()->UsePoint;
}
