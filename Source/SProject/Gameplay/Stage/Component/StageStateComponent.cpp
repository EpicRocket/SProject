
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
// include Project
#include "Types/StageTypes.h"
#include "Core/MyPlayerController.h"
#include "Gameplay/GameWorldSubsystem.h"
#include "Gameplay/Team/GameplayTeamSubsystem.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/StageTableRepository.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStateComponent)


void UStageStateComponent::InitializeComponent()
{
	Super::InitializeComponent();

	bLoadCompleted = false;
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
	/*TSoftObjectPtr<UWorld> MapPtr;
	if (auto Err = UStageTableHelper::GetStageMap(Stage.Level, MapPtr); !GameCore::IsOK(Err))
	{
		return Err;
	}

	OnLoadStage(Stage, MapPtr);*/
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

void UStageStateComponent::SetTargetLevel(AMyGameLevel* Level)
{
	TargetStage = Cast<AStageLevel>(Level);
}
