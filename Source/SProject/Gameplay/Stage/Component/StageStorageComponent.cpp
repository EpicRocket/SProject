
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

TSharedPtr<FStage> UStageStorageComponent::GetStage(int32 InStageLevel) const
{
	if (Stages.Contains(InStageLevel))
	{
		LastStageLevel = InStageLevel;
		return Stages[InStageLevel];
	}

	auto StageSubsystem = UStageSubsystem::Get(GetOwningLocalPlayer());
	if (!StageSubsystem)
	{
		GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("[UStageStorageComponent::InitializeComponent]StageSubsystem"));
		return TSharedPtr<FStage>{};
	}

	TSharedPtr<FStage> Temp = MakeShared<FStage>();
	*Temp = *StageSubsystem->GetStage(InStageLevel);
	LastStageLevel = InStageLevel;
	Stages.Add(InStageLevel, Temp);

	return Temp;
}

void UStageStorageComponent::SetStage(const FStage& InStage)
{
	if (!Stages.Contains(InStage.Level))
	{
		return;
	}
	*Stages[InStage.Level] = InStage;
}

void UStageStorageComponent::Flush()
{
	// TODO: Flush 함수 구현 - 이 기능은 실제 유저의 정보를 저장 해야 합니다.
}
