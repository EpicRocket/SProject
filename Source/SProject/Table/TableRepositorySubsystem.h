
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"

#include "TableRepositorySubsystem.generated.h"

class UTableSubsystem;

UCLASS(Abstract)
class MY_API UTableRepositorySubsystem : public UEngineSubsystem
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
