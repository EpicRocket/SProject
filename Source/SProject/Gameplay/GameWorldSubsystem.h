
#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "GameWorldSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameplay, Log, All);

class UWorld;
class ULevelStreaming;
class AMyGameLevel;
struct FLatentActionInfo;

UCLASS()
class MY_API UGameWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "게임플레이", meta = (Latent = "", LatentInfo = "LatentInfo", ReturnDisplayName = "Request Success"))
    bool RequestLoadGameWorld(const TSoftObjectPtr<UWorld> Level, bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad, FLatentActionInfo LatentInfo);

    UFUNCTION(BlueprintCallable, Category = "게임플레이")
    bool IsExistsLoadedGameWorld() const;

private:
    FString MakeSafeLevelName(const FName& LevelName) const;

    ULevelStreaming* FindAndCacheLevelStreamingObject(const FName LevelName) const;

private:
    //TMap<FString, TWeakObjectPtr<ULevelStreaming>> ;
	//TArray<TWeakObjectPtr<AMyGameLevel>> 로드중인레벨;
};
