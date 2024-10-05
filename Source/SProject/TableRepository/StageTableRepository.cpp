
#include "StageTableRepository.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
// include Project
#include "Table/TowerTable.h"
#include "TableTypes/StageTableTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageTableRepository)

UStageTableRepository* UStageTableRepository::Get()
{
	if (GEngine == NULL)
	{
		return nullptr;
	}
	return GEngine->GetEngineSubsystem<UStageTableRepository>();
}

void UStageTableRepository::Load()
{
	// TODO:	
}

void UStageTableRepository::Unload()
{
	// TODO:
}

bool UStageTableHelper::GetBuildStageTower(int32 Kind, int32 Level, FBuildStageTower& Result)
{
	auto Repository = UStageTableRepository::Get();
	if (!Repository)
	{
		return false;
	}

	auto KindTable = Repository->TowerTableRows.Find(Kind);
	if (!KindTable)
	{
		return false;
	}

	auto TowerRow = KindTable->Find(Level);
	if (!TowerRow)
	{
		return false;
	}

	auto& TowerPtr = *TowerRow;
	Result.Index = TowerPtr->Index;
	Result.Kind = TowerPtr->Kind;
	Result.Level = TowerPtr->Level;
	Result.Name = TowerPtr->Name;
	Result.Icon = TowerPtr->IconPath.LoadSynchronous();

	return true;
}
