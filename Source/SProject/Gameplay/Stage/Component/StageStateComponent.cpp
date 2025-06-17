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


#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStateComponent)


void UStageStateComponent::InitializeComponent()
{
	Super::InitializeComponent();
	bLoadCompleted = false;
}

void UStageStateComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
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

	StageLoadFlags = EStageLoadFlags::None;

	OnLoadStage(Stage, Map);
	return GameCore::Pass();
}

void UStageStateComponent::AddStageLoadFlags(EStageLoadFlags Flags, FGErrorInfo Error)
{
	if (!GameCore::IsOK(Error))
	{
		// TODO: 해당 Flags에서 오류가 발생함... 예외 처리 해줘야하는 로직 추가
		return;
	}

	StageLoadFlags |= Flags;
	if (StageLoadFlags == EStageLoadFlags::All)
	{
		StageLoadFlags = EStageLoadFlags::Complete;
		OnLoadComplete();
	}
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

FGErrorInfo UStageStateComponent::SetStageLevel(const FStage& Stage, AMyGameLevel* GameLevel)
{
	auto StageLevel = Cast<AStageLevel>(GameLevel);
	if (!StageLevel)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("GameLevel is not AStageLevel: %s"), *GameLevel->GetName()));
	}

	return StageLevel->Setup(Stage.Level);
}
