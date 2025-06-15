// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"

#include "GTableRepositorySubsystem.generated.h"

class UGTableSubsystem;
struct FLatentActionInfo;
struct FSoftObjectPath;
struct FStreamableHandle;

UCLASS(Abstract)
class GGAMECORE_API UGTableRepositorySubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
    // FTickableObjectBase
    virtual bool IsTickable() const override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual UWorld* GetTickableGameObjectWorld() const override;
    // ~FTickableObjectBase

protected:
    void RequestTask(FSoftObjectPath Path, TFunction<void()> ComplateCallback);

private:
    void ResetTask();

private:
    bool bLoaded = false;

    bool bWorking = false;
    TArray<TSharedPtr<FStreamableHandle>> Tasks;
    FThreadSafeCounter TaskCompleteCount;

};
