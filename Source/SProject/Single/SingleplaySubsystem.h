
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "SingleplaySubsystem.generated.h"


class USingleplaySaveGame;

/*
 * 싱글플레이 서브시스템
 */
UCLASS()
class SPROJECT_API USingleplaySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱글플레이")
    void LoadSaveGames();

private:
    TArray<USingleplaySaveGame> SaveGames;

    TObjectPtr<USingleplaySaveGame> SelectedSaveGame;
};
