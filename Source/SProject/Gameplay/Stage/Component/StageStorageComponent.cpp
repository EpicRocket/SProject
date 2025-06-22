
#include "StageStorageComponent.h"
// include GameCore
#include "Core/Action/GGameComponentLoadAction.h"
// include Project
#include "Types/StageTypes.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/GameplayUserPlayer.h"
#include "User/StageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStorageComponent)

void UStageStorageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	auto User = GetGameplayPlayer<AGameplayUserPlayer>();
	if (!User)
	{
		GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("[UStageStorageComponent::InitializeComponent]AGameplayUserPlayer"));
		return;
	}

	auto StageSubsystem = UStageSubsystem::Get(User->GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("[UStageStorageComponent::InitializeComponent]StageSubsystem"));
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
