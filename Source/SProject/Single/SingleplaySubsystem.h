
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "SingleplaySubsystem.generated.h"

class USingleplaySaveGameContext;
class ULocalPlayer;

DECLARE_LOG_CATEGORY_EXTERN(LogSingleplay, Log, All);

UCLASS()
class MY_API USingleplaySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	static FString SLOT_NAME;

public:
	static USingleplaySubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void Load();

	UFUNCTION(BlueprintCallable)
	USingleplaySaveGameContext* GetSaveGame() const;

private:
	UPROPERTY()
	TObjectPtr<USingleplaySaveGameContext> LoadedSaveGame;
};
