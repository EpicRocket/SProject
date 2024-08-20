
#include "StagePlayerController.h"
// include Engine
#include "Blueprint/UserWidget.h"
// include Project
#include "StageLogging.h"
#include "StageGameplayTags.h"
#include "Table/TableSubsystem.h"
#include "Table/TowerTable.h"
#include "User/DomainSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePlayerController)

bool AStagePlayerController::GetBuildTowerDatas(TArray<FTowerTableRow>& OutDatas) const
{
	OutDatas.Empty();

	auto TableSubsystem = UTableSubsystem::Get();
	if (!TableSubsystem)
	{
		UE_LOG(LogTable, Warning, TEXT("TableSubsystem이 유효하지 않습니다."));
		return false;
	}

	for (auto Data : TableSubsystem->GetTableDatas<FTowerTableRow>())
	{
		if (!Data)
		{
			continue;
		}
		OutDatas.Emplace(*Data);
	}

	return true;
}

bool AStagePlayerController::GetBuildTowerDatasForUser(TArray<FTowerTableRow>& OutDatas) const
{
	OutDatas.Empty();

	auto TableSubsystem = UTableSubsystem::Get();
	if (!TableSubsystem)
	{
		UE_LOG(LogTable, Warning, TEXT("TableSubsystem이 유효하지 않습니다."));
		return false;
	}

	auto DomainSubsystem = UDomainSubsystem::Get(GetLocalPlayer());
	if (!DomainSubsystem)
	{
		UE_LOG(LogStage, Warning, TEXT("DomainSubsystem이 유효하지 않습니다."));
		return false;
	}

	for (auto Data : TableSubsystem->GetTableDatas<FTowerTableRow>())
	{
		if (!Data)
		{
			continue;
		}

		if (!DomainSubsystem->IsBuildableTower(*Data))
		{
			continue;
		}

		OutDatas.Emplace(*Data);
	}

	return true;
}

AStagePlayerController* UStagePlayerControllerHelper::GetStagePlayerControllerToWidget(UUserWidget* UserWidget)
{
	if (UserWidget == nullptr)
	{
		return nullptr;
	}
	return UserWidget->GetOwningPlayer<AStagePlayerController>();
}
