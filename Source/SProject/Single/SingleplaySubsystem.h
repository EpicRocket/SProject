
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "SingleplaySubsystem.generated.h"

class USingleplaySaveGameContext;

DECLARE_LOG_CATEGORY_EXTERN(LogSingleplay, Log, All);

/*
 * 싱글플레이 서브시스템
 */
UCLASS()
class SPROJECT_API USingleplaySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    static FString SLOT_NAME;
    static int32 SLOT_START_INDEX;
    static int32 SLOT_MAX_COUNT;

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱글플레이")
    void LoadSaveGames();

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱글플레이")
    void SaveSelectedSaveGame();

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱그플레이")
    TArray<USingleplaySaveGameContext*> GetSaveGames() const;

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱그플레이")
    void DeleteSaveGame(USingleplaySaveGameContext* DeleteSaveGame);

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱그플레이")
    void SelectSaveGame(USingleplaySaveGameContext* SelecteSaveGame);

private:
    TArray<USingleplaySaveGameContext*> Contexts;

    TObjectPtr<USingleplaySaveGameContext> SelectedSaveGame;
};
