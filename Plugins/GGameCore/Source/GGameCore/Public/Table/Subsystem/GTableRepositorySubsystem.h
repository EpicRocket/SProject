
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"

#include "GTableRepositorySubsystem.generated.h"

class UGTableSubsystem;
struct FLatentActionInfo;
struct FSoftObjectPath;
struct FStreamableHandle;

UCLASS(Abstract)
class GGAMECORE_API UGTableRepositorySubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"))
    virtual void Load(FLatentActionInfo LatentInfo);

    UFUNCTION(BlueprintCallable)
    virtual void Unload();

protected:
    virtual void OnLoad() { /*needs implement*/ }
    virtual void OnUnload() { /*needs implement*/ }
    void RequestTask(FSoftObjectPath Path, TFunction<void()> ComplateCallback);

private:
    UFUNCTION()
    void Patch();

    void ResetTask();

private:
    bool bLoaded = false;
    TArray<TSharedPtr<FStreamableHandle>> Tasks;
    FThreadSafeCounter TaskCompleteCount;

};
 