
#include "GameWorldSubsystem.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/PlayerController.h"
#include "Misc/PackageName.h"
#include "Kismet/KismetSystemLibrary.h"

// include Project

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameWorldSubsystem)

DEFINE_LOG_CATEGORY(LogGameplay);


bool UGameWorldSubsystem::RequestLoadGameWorld(const TSoftObjectPtr<UWorld> Level, bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad, FLatentActionInfo LatentInfo)
{
	auto World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogGameplay, Error, TEXT("World is nullptr"));
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return false;
	}

	if (Level.IsNull())
	{
		UE_LOG(LogGameplay, Error, TEXT("Level is nullptr"));
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return false;
	}

	const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	auto LevelStreaming = FindAndCacheLevelStreamingObject(LevelName);

	if (LevelStreaming == nullptr)
	{
		UE_LOG(LogGameplay, Error, TEXT("레벨 스트리밍 오브젝트를 찾을 수 없습니다. [Level: %s]"), *Level.GetAssetName());
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return false;
	}

	if (LevelStreaming->IsLevelLoaded())
	{
		UE_LOG(LogGameplay, Error, TEXT("이미 로드된 레벨 입니다. [Level: %s]"), *Level.GetAssetName());
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return false;
	}

	FLatentActionManager& LatentManager = World->GetLatentActionManager();
	if (LatentManager.FindExistingAction<FStreamLevelAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) != nullptr)
	{
		UE_LOG(LogGameplay, Error, TEXT("이미 로드 중인 레벨 입니다. [Level: %s]"), *Level.GetAssetName());
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return false;
	}

	FStreamLevelAction* NewAction = new FStreamLevelAction(true, LevelName, bMakeVisibleAfterLoad, bShouldBlockOnLoad, LatentInfo, World);
	LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

	return true;
}

bool UGameWorldSubsystem::IsExistsLoadedGameWorld() const
{
	return false;
}

FString UGameWorldSubsystem::MakeSafeLevelName(const FName& LevelName) const
{
	auto World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogGameplay, Error, TEXT("World is nullptr"));
		return LevelName.ToString();
	}

	if (!World->StreamingLevelsPrefix.IsEmpty())
	{
		FString PackageName = FPackageName::GetShortName(LevelName);
		if (!PackageName.StartsWith(World->StreamingLevelsPrefix))
		{
			PackageName = World->StreamingLevelsPrefix + PackageName;
		}

		if (!FPackageName::IsShortPackageName(LevelName))
		{
			PackageName = FPackageName::GetLongPackagePath(LevelName.ToString()) / PackageName;
		}

		return PackageName;
	}

	return LevelName.ToString();
}

ULevelStreaming* UGameWorldSubsystem::FindAndCacheLevelStreamingObject(const FName LevelName) const
{
	auto World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogGameplay, Error, TEXT("World is nullptr"));
		return nullptr;
	}

	FString SearchPackageName = MakeSafeLevelName(LevelName);
	if (FPackageName::IsShortPackageName(SearchPackageName))
	{
		SearchPackageName = TEXT("/") + SearchPackageName;
	}

	for (ULevelStreaming* LevelStreaming : World->GetStreamingLevels())
	{
		if (LevelStreaming &&
			LevelStreaming->GetWorldAssetPackageName().EndsWith(SearchPackageName, ESearchCase::IgnoreCase))
		{
			return LevelStreaming;
		}
	}

	return nullptr;
}
