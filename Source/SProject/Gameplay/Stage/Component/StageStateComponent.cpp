
#include "StageStateComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LevelStreamingDynamic.h"
// include GameCore
#include "Error/GErrorManager.h"
// include Project
#include "Table/TableSubsystem.h"
#include "Table/StageInfoTable.h"
#include "Gameplay/Stage/Error/StageTableError.h"
#include "Gameplay/GameWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStateComponent)

void UStageStateComponent::InitializeComponent()
{
	Super::InitializeComponent();

	bLoadCompleted = false;
}

void UStageStateComponent::BeginPlay()
{
	Super::BeginPlay();


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

	auto GameWorldSubsystem = UWorld::GetSubsystem<UGameWorldSubsystem>(GetWorld());
	if (!GameWorldSubsystem)
	{
		return FGErrorInfo(EGErrType::Error, TEXT(""), FText{});
	}

	auto LevelStreaming = GameWorldSubsystem->GetLevelStreamingDynamic(Data->Map);
	if (!LevelStreaming)
	{
		return FGErrorInfo(EGErrType::Error, TEXT(""), FText{});
	}

	// FIXME: 로드가 되었는지 확인 해야함.


	return FGErrorInfo{};
}
