
#include "StageStateComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
// include GameCore
#include "Error/GErrorManager.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/StageInfoTable.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/Error/StageTableError.h"
#include "Gameplay/GameWorldSubsystem.h"
#include "GameFramework/MyPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStateComponent)

//////////////////////////////////////////////////////////////////////////
class FStageLoadAction : public FPendingLatentAction
{
public:
	FLatentActionInfo LatentInfo;
	TWeakObjectPtr<UWorld> WorldPtr;
	TFunction<void()> OnCompleted;
	TFunction<void()> OnFailed;

	FStageLoadAction(const FLatentActionInfo& InLatentInfo, UWorld* World, TFunction<void()> CompletedCallback, TFunction<void()> FailedCallback)
		: LatentInfo(InLatentInfo)
		, WorldPtr(World)
		, OnCompleted(CompletedCallback)
		, OnFailed(FailedCallback)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		if (!WorldPtr.IsValid())
		{
			OnFailed();
			Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
			return;
		}
		
		auto FirstPC = WorldPtr->GetFirstPlayerController();
		if (!FirstPC)
		{
			return;
		}
		else if (!FirstPC->HasActorBegunPlay())
		{
			return;
		}

		auto MyPC = Cast<AMyPlayerController>(FirstPC);
		if (!MyPC)
		{
			OnFailed();
			Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
			return;
		}

		OnCompleted();
		Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
	}

#if WITH_EDITOR
	virtual FString GetDescription() const override
	{
		return TEXT("스테이지 로드 중...");
	}
#endif
};


//////////////////////////////////////////////////////////////////////////

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

FGErrorInfo UStageStateComponent::SetLevel(int32 Level)
{
	auto Data = UTableHelper::GetData<FStageTableRow>(Level);
	if (!Data)
	{
		return FStageTableError(Level);
	}

	LoadLevel = Data->Map;
	OnLoadStreaming();

	return FGErrorInfo{};
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
	if (LatentManager.FindExistingAction<FStageLoadAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) != nullptr)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return FGErrorInfo(EGErrType::Warning, TEXT(""), FText{});
	}

	FGErrorInfo ErrorInfo;

	auto OnSuccess = [&ErrorInfo, ThisPtr = TWeakObjectPtr<UStageStateComponent>(this)]
		{
			if (ThisPtr.IsValid())
			{
				
			}
		};

	auto OnFailed = [&ErrorInfo, ThisPtr = TWeakObjectPtr<UStageStateComponent>(this)]
		{
			if (ThisPtr.IsValid())
			{
				ErrorInfo.ErrType = EGErrType::Error;
			}
		};

	FStageLoadAction* NewAction = new FStageLoadAction(LatentInfo, GetWorld(), OnSuccess, OnFailed);
	LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

	return ErrorInfo;
}

void UStageStateComponent::FailLoadLevel()
{
	// TODO: SetLevel에 실패함...
}

void UStageStateComponent::SuccessLoadLevel()
{
	auto GameWorldSubsystem = UWorld::GetSubsystem<UGameWorldSubsystem>(GetWorld());
	if (!GameWorldSubsystem)
	{
		return;
	}

	auto MyGameLevel = GameWorldSubsystem->FindLoadedLevel(LoadLevel);
	if (!MyGameLevel)
	{
		return;
	}

	auto StageLevel = Cast<AStageLevel>(MyGameLevel);
	if (!StageLevel)
	{
		return;
	}
	TargetStage = StageLevel;

	OnLoadLevelCompleted();
}

void UStageStateComponent::OnLoadLevelCompleted()
{


	K2_OnLoadLevelCompleted();
}
