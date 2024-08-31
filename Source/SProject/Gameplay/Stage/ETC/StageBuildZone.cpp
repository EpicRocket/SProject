
#include "StageBuildZone.h"
// include Project
#include "Gameplay/GameplayFunctionLibrary.h"

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
