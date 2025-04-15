
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

    UFUNCTION(BlueprintCallable, Category = "寃뚯엫?뚮젅??, meta = (Latent = "", LatentInfo = "LatentInfo", ReturnDisplayName = "Request Success"))
    bool RequestLoadGameWorld(const TSoftObjectPtr<UWorld> Level, bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad, FLatentActionInfo LatentInfo);

    // 濡쒕뱶 ???덈꺼??議댁옱?섎뒗吏 ?먮퀎
    UFUNCTION(BlueprintCallable, Category = "寃뚯엫?뚮젅??)
    bool IsExistsLoadedGameWorld() const;

    // ?덈꺼 濡쒕뱶 以묒씤吏 ?먮퀎
    UFUNCTION(BlueprintCallable, Category = "寃뚯엫?뚮젅??)
	bool IsDoingLoadGameWorld() const;

	// ?덈꺼 ?몃줈??以묒씤吏 ?먮퀎
    UFUNCTION(BlueprintCallable, Category = "寃뚯엫?뚮젅??)
	bool IsDoingUnloadGameWorld() const;

	UFUNCTION(BlueprintPure, Category = "寃뚯엫?뚮젅??)
    AMyGameLevel* FindLoadedLevel(TSoftObjectPtr<UWorld> Level);

    UFUNCTION(BlueprintPure, Category = "寃뚯엫?뚮젅??)
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
