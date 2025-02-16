
#include "StageStateComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/LatentActionManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
// include GameCore
#include "Error/GErrorManager.h"
#include "Core/GGameLoadAction.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/StageTable.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/Error/StageTableError.h"
#include "Gameplay/GameWorldSubsystem.h"

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
		OutReason = TEXT("스테이지 로딩 중...");
		return true;
	}

	return false;
}

FGErrorInfo UStageStateComponent::OnLoadStage(FLatentActionInfo LatentInfo)
{
	auto World = GetWorld();
	if (!World)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return FGErrorInfo(EGErrType::Error, TEXT(""), FText{});
	}

	FLatentActionManager& LatentManager = World->GetLatentActionManager();
	if (LatentManager.FindExistingAction<FGGameLoadAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) != nullptr)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return FGErrorInfo(EGErrType::Warning, TEXT(""), FText{});
	}

	FGErrorInfo ErrorInfo;
	auto OnSuccess = [this, World, &ErrorInfo, ThisPtr = TWeakObjectPtr<UStageStateComponent>(this)](APlayerController* PrimaryPlayerController)
		{
			if (!ThisPtr.IsValid())
			{
				return;
			}
			
			PrimaryPC = PrimaryPlayerController;
		};

	auto OnFailed = [&ErrorInfo, ThisPtr = TWeakObjectPtr<UStageStateComponent>(this)]
		{
			if (ThisPtr.IsValid())
			{
				ErrorInfo.ErrType = EGErrType::Error;
			}
		};

	FGGameLoadAction* NewAction = new FGGameLoadAction(LatentInfo, GetWorld(), OnSuccess, OnFailed);
	LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

	return ErrorInfo;
}

void UStageStateComponent::SetTargetLevel(AMyGameLevel* Level)
{
	TargetStage = Cast<AStageLevel>(Level);
}
