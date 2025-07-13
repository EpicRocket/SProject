// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "GameplayWorldSubsystem.generated.h"

class UWorld;
class ULevelStreaming;
class AGameplayLevel;
struct FLatentActionInfo;
struct FGErrorInfo;

UCLASS()
class GGAMECORE_API UGameplayWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (Latent = "", LatentInfo = "LatentInfo", ReturnDisplayName = "Error"))
	FGErrorInfo RequestLoadGameplayLevel(const TSoftObjectPtr<UWorld> Level, bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (Latent = "", LatentInfo = "LatentInfo", ReturnDisplayName = "Error"))
	FGErrorInfo RequestUnloadGameplayLevel(const TSoftObjectPtr<UWorld> Level, bool bShouldBlockOnUnload, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	bool IsExistsLoadedGameplayLevel() const;

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	bool IsDoingLoadGameplayLevel() const;

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	bool IsDoingUnloadGameplayLevel() const;

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	AGameplayLevel* FindLoadedLevel(TSoftObjectPtr<UWorld> Level);

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	AGameplayLevel* GetTopLevel();

private:
	FString MakeSafeLevelName(const FName& LevelName) const;

	ULevelStreaming* FindAndCacheLevelStreamingObject(const FName LevelName) const;

	UFUNCTION()
	void OnLevelLoaded();

	UFUNCTION()
	void OnLevelUnloaded();

private:
	TArray<TWeakObjectPtr<ULevelStreaming>> StreamingLevels;

	TArray<TWeakObjectPtr<ULevelStreaming>> ScheduledLoadedLevels;

	TArray<TWeakObjectPtr<ULevelStreaming>> ScheduledUnloadedLevels;
};
