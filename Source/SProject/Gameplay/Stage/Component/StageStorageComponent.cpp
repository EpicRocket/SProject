
#include "StageStorageComponent.h"
// include GameCore
#include "Core/Action/GGameComponentLoadAction.h"
// include Project
#include "Types/StageTypes.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Team/GameplayUserPlayer.h"
#include "User/StageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStorageComponent)

void UStageStorageComponent::OnInitialize()
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
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("StageSubsystem"));
		return;

	}

	auto LastStage = StageSubsystem->GetLastStage();
	LastStageLevel = LastStage->Level;
	Stages.Emplace(LastStage->Level, LastStage);
}

FStage UStageStorageComponent::GetLastStage() const
{
	auto Iter = Stages.Find(LastStageLevel);
	if (Iter)
	{
		return **Iter;
	}
	return FStage{};
}
