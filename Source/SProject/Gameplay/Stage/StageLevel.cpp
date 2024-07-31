
#include "StageLevel.h"
// include Project
#include "StageLogging.h"
#include "ETC/StageBuildZone.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageLevel)

void AStageLevel::BeginPlay()
{
	Super::BeginPlay();

	// TODO:
}

void AStageLevel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// TODO:

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

	int32 Position = INDEX_NONE;
	for (const auto& Tag : BuildZonePtr->Tags)
	{
		FString TagString = Tag.ToString();
		FString* Left = nullptr;
		FString* Right = nullptr;
		if (!TagString.Split(TEXT("."), Left, Right))
		{
			continue;
		}

		if (Left == nullptr || Right == nullptr)
		{
			continue;
		}

		if (!Left->Contains(TEXT("Position")))
		{
			continue;
		}

		Position = FCString::Atoi(**Right);
		break;
	}

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
