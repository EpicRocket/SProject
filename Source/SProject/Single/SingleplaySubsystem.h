
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "SingleplaySubsystem.generated.h"

class USingleplaySaveGameContext;
class ULocalPlayer;

DECLARE_LOG_CATEGORY_EXTERN(LogSingleplay, Log, All);

/*
 * ?깃??뚮젅???쒕툕?쒖뒪??
 */
UCLASS()
class MY_API USingleplaySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    static FString SLOT_NAME;

public:
    static USingleplaySubsystem* Get(const UObject* WorldContextObject);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "?깃??뚮젅??)
    void Load();

    UFUNCTION(BlueprintCallable, Category = "?깃??뚮젅??)
    USingleplaySaveGameContext* GetSaveGame() const;

private:
    UPROPERTY()
    TObjectPtr<USingleplaySaveGameContext> LoadedSaveGame;
};
