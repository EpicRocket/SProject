
#include "Table/Subsystem/GTableRepositorySubsystem.h"
// include Engine
#include "Engine/LatentActionManager.h"
#include "Engine/AssetManager.h"
#include "UObject/SoftObjectPath.h"
#include "Subsystems/SubsystemCollection.h"
// include GameCore
#include "Table/Subsystem/GTableSubsystem.h"

void UGTableRepositorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (auto Subsystem = Collection.InitializeDependency<UGTableSubsystem>())
	{
		Subsystem->TableLoadCompleted.AddWeakLambda(this, [this, ThisPtr = TWeakObjectPtr<ThisClass>(this)]()
			{
				if (ThisPtr.IsValid())
				{
					Patch();
				}
			});
	}
}

void UGTableRepositorySubsystem::Load(FLatentActionInfo LatentInfo)
{
	bLoaded = true;
	ResetTask();
	//OnLoad();
}

void UGTableRepositorySubsystem::Unload()
{
	bLoaded = false;
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
		ResetTask();
		OnLoad();
	}
}

void UGTableRepositorySubsystem::ResetTask()
{
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
