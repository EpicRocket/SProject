
#include "StageLevel.h"
// include Project
#include "StageLogging.h"
#include "ETC/StageBuildZone.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageLevel)

void AStageLevel::BeginPlay()
{
	Super::BeginPlay();

	OnInitailize();

	// TODO:
}

void AStageLevel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// TODO:
	BuildZones.Empty();

	Super::EndPlay(EndPlayReason);
}

void AStageLevel::AddBuildZone(AStageBuildZone* BuildZone)
{
	TWeakObjectPtr<AStageBuildZone> BuildZonePtr = BuildZone;

	if (!BuildZonePtr.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone을 찾지 못하였습니다."));
		return;
	}

	if (BuildZonePtr->Tags.IsEmpty())
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone(%s)에 부여된 태그가 없습니다."), *BuildZonePtr->GetFName().ToString());
		return;
	}

	int32 Position = BuildZone->GetPosition();
	if (Position == INDEX_NONE)
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone(%s)에 Position 태그가 없습니다."), *BuildZonePtr->GetFName().ToString());
		return;
	}

	if (BuildZones.Contains(Position))
	{
		UE_LOG(LogStage, Warning, TEXT("이미 BuildZone이 존재합니다. 기존 BuildZone이 덮어집니다. Position(%d)"), Position);
	}

	BuildZones.Emplace(Position, BuildZonePtr);
}

AStageBuildZone* AStageLevel::GetBuildZone(int32 Position) const
{
	if (!BuildZones.Contains(Position))
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone을 찾지 못하였습니다. [Level: %s][Position: %d]"), *GetFName().ToString(), Position);
		return nullptr;
	}

	auto& BuildZone = BuildZones[Position];
	if (!BuildZone.IsValid())
	{
		UE_LOG(LogStage, Warning, TEXT("BuildZone이 유효하지 않습니다. [Level: %s][Position: %d]"), *GetFName().ToString(), Position);
		return nullptr;
	}

	return BuildZone.Get();
}

//ATowerBase* AStageLevel::GetTower(int32 Position) const
//{
//	return nullptr;
//}
