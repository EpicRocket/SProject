// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Table/Subsystem/GTableRepositorySubsystem.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/AssetManager.h"
#include "UObject/SoftObjectPath.h"
#include "Subsystems/SubsystemCollection.h"
// include GameCore
#include "Table/Subsystem/GTableSubsystem.h"

/////////////////////////////////////////////////////////////////
// UGTableRepositorySubsystem
////////////////////////////////////////////////////////////////

void UGTableRepositorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	auto TableSubsystem = UGTableSubsystem::Get();
	if (!TableSubsystem)
	{
		return;
	}

	TableSubsystem->TableLoadCompleted.AddWeakLambda(this, [this, ThisPtr = TWeakObjectPtr<ThisClass>(this)]()
		{
			if (ThisPtr.IsValid())
			{
				Patch();
			}
		});
}

void UGTableRepositorySubsystem::Deinitialize()
{
	auto TableSubsystem = UGTableSubsystem::Get();
	if (!TableSubsystem)
	{
		return;
	}

	TableSubsystem->TableLoadCompleted.RemoveAll(this);

	Unload();
}

bool UGTableRepositorySubsystem::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject) && bWorking;
}

void UGTableRepositorySubsystem::Tick(float DeltaTime)
{
	if (!bLoaded || !bWorking)
	{
		return;
	}

	int32 TaskCount = Tasks.Num();
	bool bComplete
		=	TaskCount == 0
		||	TaskCount == TaskCompleteCount.GetValue()
		;

	if (bComplete)
	{
		ResetTask();
		OnTableRepositoryLoaded.Broadcast();
	}
}

TStatId UGTableRepositorySubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGTableRepositorySubsystem, STATGROUP_Tickables);
}

UWorld* UGTableRepositorySubsystem::GetTickableGameObjectWorld() const
{
	return GetGameInstance()->GetWorld();
}

void UGTableRepositorySubsystem::Load()
{
	bLoaded = true;
	ResetTask();
	OnLoad();

	bWorking = true;
	OnTableRepositoryLoading.Broadcast();
}

void UGTableRepositorySubsystem::Unload()
{
	bLoaded = false;
	ResetTask();
	OnUnload();
}

void UGTableRepositorySubsystem::RequestTask(FSoftObjectPath Path, TFunction<void()> ComplateCallback)
{
	if (Path.IsNull())
	{
		UE_LOG(LogGameTable, Warning, TEXT("Task request failed: Path is empty"));
		return;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	auto Handle = Streamable.RequestAsyncLoad(
		Path,
		FStreamableDelegate::CreateLambda([this, ThisPtr = TWeakObjectPtr<UGTableRepositorySubsystem>(this), ComplateCallback]()
			{
				if (ThisPtr.IsValid())
				{
					TaskCompleteCount.Increment();
					if (ComplateCallback)
					{
						ComplateCallback();
					}
				}
			})
	);

	Tasks.Emplace(Handle);
}

void UGTableRepositorySubsystem::Patch()
{
	if (bLoaded)
	{
		Load();
	}
}

void UGTableRepositorySubsystem::ResetTask()
{
	bWorking = false;
	for (TSharedPtr<FStreamableHandle> Handle : Tasks)
	{
		if (Handle.IsValid())
		{
			Handle->CancelHandle();
		}
	}
	Tasks.Empty();
	TaskCompleteCount.Reset();
}
