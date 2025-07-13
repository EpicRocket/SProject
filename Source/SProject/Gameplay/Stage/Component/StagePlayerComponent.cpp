
#include "StagePlayerComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
// include GameCore
#include "GMessage/GMessage.h"
#include "Table/GTableHelper.h"
// include Project
#include "Core/MyPlayerController.h"
#include "Types/StageTypes.h"
#include "User/StageSubsystem.h"
#include "Table/ConstTable.h"
#include "Table/StageTable.h"
#include "Gameplay/Team/GameplayTeamSubsystem.h"
#include "Gameplay/GameplayUserPlayer.h"
#include "Gameplay/Stage/Message/StagePlayerEventMessage.h"
#include "Gameplay/Stage/StageTableRepository.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePlayerComponent)

FGErrorInfo UStagePlayerComponent::SetDefaults()
{
	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("[UStagePlayerComponent::SetDefaults()]AGameplayUserPlayer"));
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("[UStagePlayerComponent::SetDefaults()]UStageSubsystem"));
	}

	auto LastStage = StageSubsystem->GetLastStage();

	auto StageRow = UGTableHelper::GetTableData<FStageTableRow>(LastStage->Level);
	if (!StageRow)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("[UStagePlayerComponent::SetDefaults()]FStageTableRow find Level %d"), LastStage->Level));
	}

	LastStage->Towers.Empty();
	LastStage->Hp = GetDefault<UConstSettings>()->UserHp;
	LastStage->UsePoint = StageRow->UsePoint;

	return GameCore::Pass();
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
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("[UStagePlayerComponent::NewStart()]UGMessageSubsystem"));
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
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("[UStagePlayerComponent::Restart()]UGMessageSubsystem"));
	}

	FStagePlayerRestartMessage Message;
	GMessageSubsystem->BroadcastMessage(Stage::Tag_Gameplay_Stage_Player_Restart, Message);

	return GameCore::Pass();
}

void UStagePlayerComponent::SetHp(int32 NewHp)
{
	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		GameCore::Throw(GameErr::ACTOR_INVALID, *FString::Printf(TEXT("[UStagePlayerComponent::SetHp(NewHp:%d)]AGameplayUserPlayer"), NewHp));
		return;
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, *FString::Printf(TEXT("[UStagePlayerComponent::SetHp(NewHp:%d)]UStageSubsystem"), NewHp));
		return;
	}

	auto GMessageSubsystem = UGameInstance::GetSubsystem<UGMessageSubsystem>(GetGameInstance<UGameInstance>());
	if (!GMessageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, *FString::Printf(TEXT("[UStagePlayerComponent::SetHp(NewHp:%d)]UGMessageSubsystem"), NewHp));
		return;
	}

	FStagePlayerHpMessage Message;
	Message.OldValue = StageSubsystem->GetLastStage()->Hp;
	StageSubsystem->GetLastStage()->Hp = NewHp;
	Message.NewValue = StageSubsystem->GetLastStage()->Hp;
	GMessageSubsystem->BroadcastMessage(Stage::Tag_Gameplay_Stage_Player_Hp_Changed, Message);
}

void UStagePlayerComponent::AddHp(int32 AddHp)
{
	SetHp(GetHp() + AddHp);
}

int32 UStagePlayerComponent::GetHp() const
{
	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("[UStagePlayerComponent::GetHp()]AGameplayUserPlayer"));
		return 0;
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("[UStagePlayerComponent::GetHp()]UStageSubsystem"));
		return 0;
	}

	return StageSubsystem->GetLastStage()->Hp;
}

void UStagePlayerComponent::SetUsePoint(int64 NewUsePoint)
{
	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		GameCore::Throw(GameErr::ACTOR_INVALID, *FString::Printf(TEXT("[UStagePlayerComponent::SetHp(NewHp:%d)]AGameplayUserPlayer"), NewUsePoint));
		return;
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, *FString::Printf(TEXT("[UStagePlayerComponent::SetHp(NewHp:%d)]UStageSubsystem"), NewUsePoint));
		return;
	}

	auto GMessageSubsystem = UGameInstance::GetSubsystem<UGMessageSubsystem>(GetGameInstance<UGameInstance>());
	if (!GMessageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, *FString::Printf(TEXT("[UStagePlayerComponent::SetHp(NewHp:%d)]UGMessageSubsystem"), NewUsePoint));
		return;
	}

	FStagePlayerUsePointMessage Message;
	Message.OldValue = StageSubsystem->GetLastStage()->UsePoint;
	StageSubsystem->GetLastStage()->UsePoint = NewUsePoint;
	Message.NewValue = StageSubsystem->GetLastStage()->UsePoint;
	GMessageSubsystem->BroadcastMessage(Stage::Tag_Gameplay_Stage_Player_UsePoint_Changed, Message);
}

void UStagePlayerComponent::AddUsePoint(int64 AddUsePoint)
{
	SetUsePoint(GetUsePoint() + AddUsePoint);
}

int64 UStagePlayerComponent::GetUsePoint() const
{
	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("[UStagePlayerComponent::GetUsePoint()]AGameplayUserPlayer"));
		return 0;
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("[UStagePlayerComponent::GetUsePoint()]UStageSubsystem"));
		return 0;
	}

	return StageSubsystem->GetLastStage()->UsePoint;
}
