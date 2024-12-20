﻿
#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "GameWorldSubsystem.generated.h"

class UWorld;
class ULevelStreaming;
class AMyGameLevel;
struct FLatentActionInfo;

UCLASS()
class MY_API UGameWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

    UFUNCTION(BlueprintCallable, Category = "게임플레이", meta = (Latent = "", LatentInfo = "LatentInfo", ReturnDisplayName = "Request Success"))
    bool RequestLoadGameWorld(const TSoftObjectPtr<UWorld> Level, bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad, FLatentActionInfo LatentInfo);

    // 로드 된 레벨이 존재하는지 판별
    UFUNCTION(BlueprintCallable, Category = "게임플레이")
    bool IsExistsLoadedGameWorld() const;

    // 레벨 로드 중인지 판별
    UFUNCTION(BlueprintCallable, Category = "게임플레이")
	bool IsDoingLoadGameWorld() const;

	// 레벨 언로드 중인지 판별
    UFUNCTION(BlueprintCallable, Category = "게임플레이")
	bool IsDoingUnloadGameWorld() const;

	UFUNCTION(BlueprintPure, Category = "게임플레이")
    AMyGameLevel* FindLoadedLevel(TSoftObjectPtr<UWorld> Level);

    UFUNCTION(BlueprintPure, Category = "게임플레이")
    AMyGameLevel* GetTopLevel();

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
