
#include "StageBuildZone.h"
// include Project
#include "Gameplay/GameplayLogging.h"
#include "Table/TableSubsystem.h"
#include "TableRepository/StageTableRepository.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageBuildZone)

int32 AStageBuildZone::GetPosition() const
{
	int32 Position = INDEX_NONE;
	for (const auto& Tag : Tags)
	{
		FString TagString = Tag.ToString();
		FString Left, Right;
		if (!TagString.Split(TEXT("."), &Left, &Right))
		{
			continue;
		}

		if (!Left.Contains(TEXT("Position")))
		{
			continue;
		}

		Position = FCString::Atoi(*Right);
		break;
	}

	return Position;
}

TArray<FBuildStageTower> AStageBuildZone::GetBuildTower() const
{
	TArray<FBuildStageTower> Result;

	if (!BuildZoneData)
	{
		UE_LOG(LogGameplay, Warning, TEXT("Actor(%s)의 BuildZoneData를 찾을 수 없습니다."), *GetFName().ToString());
		return Result;
	}

	for (auto& Content : BuildZoneData->BuildContents)
	{
		FBuildStageTower Tower;
		if (!UStageTableHelper::GetBuildStageTower(Content.TowerType, Content.Kind, Content.Level, Tower))
		{
			continue;
		}
		Result.Emplace(Tower);
	}

	return Result;
}

void AStageBuildZone::Reset()
{
	OnReset();
}

void AStageBuildZone::Select()
{
	OnSelect();
}

void AStageBuildZone::Deselect()
{
	OnDeselect();
}

//void AStageBuildZone::Build(TSubclassOf<ATowerBase> TowerClass)
//{
//	if (IsValid(SpawendTower))
//	{
//		UGameplayFunctionLibrary::RemoveUnit(SpawendTower);
//	}
//
//	auto SpawnedUnit = UGameplayFunctionLibrary::SpawnUnit(TowerClass, GetTransform());
//	if (!IsValid(SpawnedUnit))
//	{
//		UE_LOG(LogTemp, Error, TEXT("타워 생성에 실패하였습니다. [Object: %s][Position: %d]"), *GetFName().ToString(), GetPosition());
//		return;
//	}
//
//	SpawendTower = Cast<ATowerBase>(SpawnedUnit);
//	if (!IsValid(SpawendTower))
//	{
//		UGameplayFunctionLibrary::RemoveUnit(SpawnedUnit);
//		UE_LOG(LogTemp, Error, TEXT("타워 캐스트에 실패하였습니다. [Object: %s][Position: %d]"), *GetFName().ToString(), GetPosition());
//		return;
//	}
//
//	OnBuild(SpawendTower);
//}
//
//ATowerBase* AStageBuildZone::GetSpawendTower() const
//{
//	if (IsValid(SpawendTower))
//	{
//		return SpawendTower;
//	}
//	return nullptr;
//}
