// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageStateComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/LatentActionManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
// include GameCore
#include "Error/GError.h"
#include "Core/Action/GGameLoadAction.h"
#include "Table/GTableHelper.h"
// include Project
#include "Core/MyPlayerController.h"
#include "Types/StageTypes.h"
#include "Table/StageTable.h"
#include "Gameplay/Stage/StageLogging.h"
#include "Gameplay/GameWorldSubsystem.h"
#include "Gameplay/Team/GameplayTeamSubsystem.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageTableRepository.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStateComponent)


void UStageStateComponent::InitializeComponent()
{
	Super::InitializeComponent();

	bLoadCompleted = false;
	auto StageTableRepo = UStageTableRepository::Get(this);
	if (!StageTableRepo)
	{
		UE_LOG(LogStage, Error, TEXT("UStageTableRepository is not found!"));
		return;
	}
	StageTableRepo->OnTableRepositoryLoading.AddDynamic(this, &UStageStateComponent::OnTableLoading);
	StageTableRepo->OnTableRepositoryLoaded.AddDynamic(this, &UStageStateComponent::OnTableLoaded);
}

void UStageStateComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	auto StageTableRepo = UStageTableRepository::Get(this);
	if (!StageTableRepo)
	{
		UE_LOG(LogStage, Error, TEXT("UStageTableRepository is not found!"));
		return;
	}
	StageTableRepo->OnTableRepositoryLoading.RemoveDynamic(this, &UStageStateComponent::OnTableLoading);
	StageTableRepo->OnTableRepositoryLoaded.RemoveDynamic(this, &UStageStateComponent::OnTableLoaded);
}

bool UStageStateComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (!bLoadCompleted)
	{
		OutReason = TEXT("로딩중입니다..");
		return true;
	}

	return false;
}

FGErrorInfo UStageStateComponent::LoadStage(const FStage& Stage)
{
	auto Row = UGTableHelper::GetTableData<FStageTableRow>(Stage.Level);
	if (!Row)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("FStageTableRow find Level %d"), Stage.Level));
	}

	TSoftObjectPtr<UWorld> Map = Row->Map;
	if (Map.IsNull())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("Map is empty: Level:%d"), Stage.Level));
	}

	if (!Map.IsValid())
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("Map is invalid: Level:%d"), Stage.Level));
	}

	OnLoadStage(Stage, Map);
	return GameCore::Pass();
}

FGErrorInfo UStageStateComponent::WaitForPrimaryPlayerController(FLatentActionInfo LatentInfo)
{
	auto World = GetWorld();
	if (!World)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Throw(GameErr::WORLD_INVALID);
	}
	
	FGErrorInfo ErrorInfo;
	auto OnSuccess = [this, World, &ErrorInfo](APlayerController* PrimaryPlayerController)
		{
			PrimaryPC = PrimaryPlayerController;
			if (auto Subsystem = World->GetSubsystem<UGameplayTeamSubsystem>())
			{
				if (auto MyPC = Cast<AMyPlayerController>(PrimaryPlayerController))
				{
					auto TeamID = Subsystem->IssusePlayerTeamID(PrimaryPlayerController);
					if (TeamID == 255)
					{
						ErrorInfo = GameCore::Throw(GameErr::VALUE_INVALID, TEXT("발급한 팀 ID가 존재하지 않습니다."));
						return;
					}

					MyPC->SetGenericTeamId(TeamID);
				}
			}
		};
	auto NewAction = new FGGameLoadAction(LatentInfo, GetWorld(), OnSuccess, [&ErrorInfo](FGErrorInfo Err) {ErrorInfo = Err; });
	World->GetLatentActionManager().AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

	return ErrorInfo;
}

void UStageStateComponent::OnTableLoading()
{
}

void UStageStateComponent::OnTableLoaded()
{
}
