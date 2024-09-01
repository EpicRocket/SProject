
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"

#include "SingleplaySubsystem.generated.h"

class USingleplaySaveGameContext;
class ULocalPlayer;

DECLARE_LOG_CATEGORY_EXTERN(LogSingleplay, Log, All);

/*
 * 싱글플레이 서브시스템
 */
UCLASS()
class MY_API USingleplaySubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

    static FString SLOT_NAME;

public:
    static USingleplaySubsystem* Get(const ULocalPlayer* LocalPlayer);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "싱글플레이")
    void Load();

    UFUNCTION(BlueprintCallable, Category = "싱글플레이")
    USingleplaySaveGameContext* GetSaveGame() const;

private:
    UPROPERTY()
    TObjectPtr<USingleplaySaveGameContext> LoadedSaveGame;
};
