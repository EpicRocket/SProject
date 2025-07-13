// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Gameplay/Subsystem/GameplayWorldSubsystem.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/LatentActionManager.h"
#include "Engine/LevelStreaming.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/Level.h"
#include "GameFramework/PlayerController.h"
#include "Misc/PackageName.h"
#include "Kismet/KismetSystemLibrary.h"
// include GameCore
#include "Gameplay/GameplayLevel.h"
#include "Error/GError.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayWorldSubsystem)


void UGameplayWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	StreamingLevels.Empty();
	ScheduledLoadedLevels.Empty();
	ScheduledUnloadedLevels.Empty();

	for (ULevelStreaming* LevelStreaming : InWorld.GetStreamingLevels())
	{
		if (!LevelStreaming)
		{
			continue;
		}

		LevelStreaming->OnLevelLoaded.AddDynamic(this, &UGameplayWorldSubsystem::OnLevelLoaded);
		LevelStreaming->OnLevelUnloaded.AddDynamic(this, &UGameplayWorldSubsystem::OnLevelUnloaded);

		StreamingLevels.Emplace(LevelStreaming);
	}
}

FGErrorInfo UGameplayWorldSubsystem::RequestLoadGameplayLevel(const TSoftObjectPtr<UWorld> Level, bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad, FLatentActionInfo LatentInfo)
{
	auto World = GetWorld();
	if (!World)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Throw(GameErr::WORLD_INVALID);
	}

	if (Level.IsNull())
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Throw(GameErr::WORLD_INVALID, TEXT("Level is null"));
	}

	const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	auto LevelStreaming = FindAndCacheLevelStreamingObject(LevelName);

	if (!LevelStreaming)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Throw(GameErr::WORLD_INVALID, TEXT("LevelStreaming is null"));
	}

	if (LevelStreaming->IsLevelLoaded())
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Pass();
	}

	FLatentActionManager& LatentManager = World->GetLatentActionManager();
	if (LatentManager.FindExistingAction<FStreamLevelAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) != nullptr)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Pass();
	}

	ScheduledLoadedLevels.Emplace(LevelStreaming);

	FStreamLevelAction* NewAction = new FStreamLevelAction(true, LevelName, bMakeVisibleAfterLoad, bShouldBlockOnLoad, LatentInfo, World);
	LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

	return GameCore::Pass();
}

FGErrorInfo UGameplayWorldSubsystem::RequestUnloadGameplayLevel(const TSoftObjectPtr<UWorld> Level, bool bShouldBlockOnUnload, FLatentActionInfo LatentInfo)
{
	auto World = GetWorld();
	if (!World)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Throw(GameErr::WORLD_INVALID);
	}

	const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	auto LevelStreaming = FindAndCacheLevelStreamingObject(LevelName);

	if (!LevelStreaming)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Throw(GameErr::WORLD_INVALID, TEXT("LevelStreaming is null"));
	}

	if (!LevelStreaming->IsLevelLoaded())
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Pass();
	}

	FLatentActionManager& LatentManager = World->GetLatentActionManager();
	if (LatentManager.FindExistingAction<FStreamLevelAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) != nullptr)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Pass();
	}

	ScheduledUnloadedLevels.Emplace(LevelStreaming);

	FStreamLevelAction* NewAction = new FStreamLevelAction(false, LevelName, false, bShouldBlockOnUnload, LatentInfo, World);
	LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

	return GameCore::Pass();
}

bool UGameplayWorldSubsystem::IsExistsLoadedGameplayLevel() const
{
	int32 LoadedLevelCount = 0;

	for (const auto& LevelStreaming : StreamingLevels)
	{
		if (LevelStreaming.IsValid() && LevelStreaming->IsLevelLoaded())
		{
			LoadedLevelCount++;
		}
	}

	return LoadedLevelCount > 0;
}

bool UGameplayWorldSubsystem::IsDoingLoadGameplayLevel() const
{
	return ScheduledLoadedLevels.Num() > 0;
}

bool UGameplayWorldSubsystem::IsDoingUnloadGameplayLevel() const
{
	return ScheduledUnloadedLevels.Num() > 0;
}

AGameplayLevel* UGameplayWorldSubsystem::FindLoadedLevel(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	auto LevelStreaming = FindAndCacheLevelStreamingObject(LevelName);

	if (LevelStreaming == nullptr)
	{
		return nullptr;
	}

	if (!LevelStreaming->IsLevelLoaded())
	{
		return nullptr;
	}

	return Cast<AGameplayLevel>(LevelStreaming->GetLevelScriptActor());
}

AGameplayLevel* UGameplayWorldSubsystem::GetTopLevel()
{
	return Cast<AGameplayLevel>(GetWorld()->GetCurrentLevel()->GetLevelScriptActor());
}

FString UGameplayWorldSubsystem::MakeSafeLevelName(const FName& LevelName) const
{
	auto World = GetWorld();

	if (World == nullptr)
	{
		//UE_LOG(LogGameplay, Error, TEXT("World is nullptr"));
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

ULevelStreaming* UGameplayWorldSubsystem::FindAndCacheLevelStreamingObject(const FName LevelName) const
{
	auto World = GetWorld();

	if (World == nullptr)
	{
		//UE_LOG(LogGameplay, Error, TEXT("World is nullptr"));
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

void UGameplayWorldSubsystem::OnLevelLoaded()
{
	for (const auto& LevelStreaming : StreamingLevels)
	{
		if (!LevelStreaming.IsValid())
		{
			continue;
		}

		if (!LevelStreaming->IsLevelLoaded())
		{
			continue;
		}

		if (ScheduledLoadedLevels.Contains(LevelStreaming))
		{
			ScheduledLoadedLevels.Remove(LevelStreaming);
		}
	}
}

void UGameplayWorldSubsystem::OnLevelUnloaded()
{
	for (const auto& LevelStreaming : StreamingLevels)
	{
		if (!LevelStreaming.IsValid())
		{
			continue;
		}

		if (LevelStreaming->IsLevelLoaded())
		{
			continue;
		}

		if (ScheduledUnloadedLevels.Contains(LevelStreaming))
		{
			ScheduledUnloadedLevels.Remove(LevelStreaming);
		}
	}
}
