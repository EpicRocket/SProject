﻿
#include "StageBuildZone.h"
// include Engine
#include "Components/BoxComponent.h"
#include "Components/ChildActorComponent.h"
// include Project
#include "Gameplay/GameplayLogging.h"
#include "Table/TableSubsystem.h"
#include "TableRepository/StageTableRepository.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageBuildZone)

FName AStageBuildZone::InteractionComponentName = TEXT("InteractionComponent");
FName AStageBuildZone::ChildActorComponentName = TEXT("ChildActorComponent");

AStageBuildZone::AStageBuildZone()
{
	InteractionComponent = CreateDefaultSubobject<UBoxComponent>(InteractionComponentName);
	InteractionComponent->SetBoxExtent(FVector(100.0f, 100.0f, 150.0f));
	static FName InteractionCollsionProfileName = FName(TEXT("UI"));
	InteractionComponent->SetCollisionProfileName(InteractionCollsionProfileName);

	ChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(ChildActorComponentName);
	ChildActorComponent->SetupAttachment(InteractionComponent);
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
