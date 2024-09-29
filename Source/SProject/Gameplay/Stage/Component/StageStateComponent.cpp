
#include "StageStateComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"
#include "Kismet/KismetSystemLibrary.h"
// include GameCore
#include "Error/GErrorManager.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/StageInfoTable.h"
#include "Gameplay/Stage/StageLevel.h"
#include "Gameplay/Stage/Error/StageTableError.h"
#include "Gameplay/GameWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStateComponent)

//////////////////////////////////////////////////////////////////////////
class FStageLoadAction : public FPendingLatentAction
{
public:
	FLatentActionInfo LatentInfo;

	FStageLoadAction(const FLatentActionInfo& InLatentInfo)
		: LatentInfo(InLatentInfo)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{

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

	FStageLoadAction* NewAction = new FStageLoadAction(LatentInfo);
	LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

	return FGErrorInfo{};
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
