
#include "StagePlayerComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
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
#include "Gameplay/Stage/Message/StagePlayerEventMessage.h"
#include "Gameplay/Stage/StageTableRepository.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePlayerComponent)

FGErrorInfo UStagePlayerComponent::SetDefaults()
{
	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("AGameplayUserPlayer"));
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
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
	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("AGameplayUserPlayer"));
		return;
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
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
		GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("AGameplayUserPlayer"));
		return 0;
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UStageSubsystem"));
		return 0;
	}

	return StageSubsystem->GetLastStage()->Hp;
}

void UStagePlayerComponent::SetUsePoint(int64 NewUsePoint)
{
	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("AGameplayUserPlayer"));
		return;
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
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

void UStagePlayerComponent::AddUsePoint(int64 AddUsePoint)
{
	SetUsePoint(GetUsePoint() + AddUsePoint);
}

int64 UStagePlayerComponent::GetUsePoint() const
{
	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("AGameplayUserPlayer"));
		return 0;
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UStageSubsystem"));
		return 0;
	}

	return StageSubsystem->GetLastStage()->UsePoint;
}
