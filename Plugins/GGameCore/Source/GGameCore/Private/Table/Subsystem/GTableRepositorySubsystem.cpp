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

void UGTableRepositorySubsystem::Deinitialize()
{
	for (auto& [Id, Task] : Tasks)
	{
		if (Task.IsValid())
		{
			Task->ReleaseHandle();
		}
		TaskIdPool.Release(Id);
	}

	Tasks.Empty();
}

void UGTableRepositorySubsystem::RequestTasks(TArray<FSoftObjectPath> RawAssetList, TUniqueFunction<void()> CompleteCallback)
{
	if (RawAssetList.Num() == 0)
	{
		if (CompleteCallback)
		{
			CompleteCallback();
		}
		return;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	int32 Id = TaskIdPool.GetNext();
	TSharedPtr<TUniqueFunction<void()>> CallbackPtr = MakeShared<TUniqueFunction<void()>>(MoveTemp(CompleteCallback));

	auto Handle = Streamable.RequestAsyncLoad(
		RawAssetList,
		FStreamableDelegate::CreateLambda([this, Id, ThisPtr = TWeakObjectPtr<UGTableRepositorySubsystem>(this), CallbackPtr]()
			{
				if (ThisPtr.IsValid())
				{
					if (CallbackPtr.IsValid() && *CallbackPtr)
					{
						(*CallbackPtr)();
					}

					Tasks.Remove(Id);
					TaskIdPool.Release(Id);
				}
			})
	);

	Tasks.Emplace(Id, Handle);
}
