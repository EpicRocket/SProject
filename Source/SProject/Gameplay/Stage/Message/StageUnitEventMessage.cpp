// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageUnitEventMessage.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GMessage/Subsystem/GMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageUnitEventMessage)

namespace Stage
{
	UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_Stage_Unit_Attack_Player, "Gameplay.Stage.Unit.Attack.Player");
}

FGameplayTag UStageUnitEvent::GetEventTag() const
{
	return FGameplayTag();
}

AStageUnitCharacter* UStageUnitEvent::GetSource() const
{
	return Source.Get();
}

void Stage::SendUnitEvent(const UObject* WorldContextObject, UStageUnitEvent* Event)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	auto World = WorldContextObject->GetWorld();
	if (!World)
	{
		return;
	}

	auto GMessageSubsystem = UGameInstance::GetSubsystem<UGMessageSubsystem>(World->GetGameInstance());
	if (!GMessageSubsystem)
	{
		return;
	}

	FGameplayTag Channel = Event->GetEventTag();
	if (!Channel.IsValid())
	{
		return;
	}

	GMessageSubsystem->BroadcastMessage<FStageUnitEventMessage>(Channel, FStageUnitEventMessage{ Event });
}
