
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

	auto StageSubsystem = UStageSubsystem::Get(GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("[UStageStorageComponent::InitializeComponent]StageSubsystem"));
		return;
	}

	// NOTE. 첫 스테이지에 대한 정보 초기화
	GetStage(*StageSubsystem->GetLastStageLevel());
}

int32 UStageStorageComponent::GetLastStageLevel() const
{
	return LastStageLevel;
}

TSharedPtr<FStage> UStageStorageComponent::GetStage(int32 StageLevel) const
{
	if (Stages.Contains(StageLevel))
	{
		LastStageLevel = StageLevel;
		return Stages[StageLevel];
	}

	auto StageSubsystem = UStageSubsystem::Get(GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("[UStageStorageComponent::InitializeComponent]StageSubsystem"));
		return TSharedPtr<FStage>{};
	}

	TSharedPtr<FStage> Temp = MakeShared<FStage>();
	*Temp = *StageSubsystem->GetStage(StageLevel);
	LastStageLevel = StageLevel;
	Stages.Add(StageLevel, Temp);

	return Temp;
}
