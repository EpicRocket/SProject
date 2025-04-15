
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

	UFUNCTION(BlueprintCallable, meta = (Latent = "", LatentInfo = "LatentInfo", ReturnDisplayName = "Request Success"))
	bool RequestLoadGameWorld(const TSoftObjectPtr<UWorld> Level, bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable)
	bool IsExistsLoadedGameWorld() const;

	UFUNCTION(BlueprintCallable)
	bool IsDoingLoadGameWorld() const;

	UFUNCTION(BlueprintCallable)
	bool IsDoingUnloadGameWorld() const;

	UFUNCTION(BlueprintPure)
	AMyGameLevel* FindLoadedLevel(TSoftObjectPtr<UWorld> Level);

	UFUNCTION(BlueprintPure)
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
