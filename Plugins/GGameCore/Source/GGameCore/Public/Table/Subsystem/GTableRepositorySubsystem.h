
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"

#include "GTableRepositorySubsystem.generated.h"

class UGTableSubsystem;

UCLASS(Abstract)
class GGAMECORE_API UGTableRepositorySubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    virtual void Load();

    UFUNCTION(BlueprintCallable)
    virtual void Unload();

private:
    UFUNCTION()
    void Patch();

private:
    bool bLoaded = false;
};
