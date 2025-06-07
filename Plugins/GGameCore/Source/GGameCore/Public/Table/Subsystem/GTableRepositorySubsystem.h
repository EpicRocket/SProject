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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTableRepositoryLoaded);

UCLASS(Abstract)
class GGAMECORE_API UGTableRepositorySubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
    // USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
    // ~USubsystem

    // FTickableObjectBase
    virtual bool IsTickable() const override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual UWorld* GetTickableGameObjectWorld() const override;
    // ~FTickableObjectBase

public:
    UFUNCTION(BlueprintCallable)
    virtual void Load();

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

public:
    UPROPERTY(BlueprintAssignable)
    FOnTableRepositoryLoaded OnTableRepositoryLoaded;

private:
    bool bLoaded = false;

    bool bWorking = false;
    TArray<TSharedPtr<FStreamableHandle>> Tasks;
    FThreadSafeCounter TaskCompleteCount;

};
 