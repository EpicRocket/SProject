
#include "SuperAssetManager.h"

#include "SProject.h"
#include "Misc/ScopedSlowTask.h"
#include "Stats/StatsMisc.h"

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("SProject.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(USuperAssetManager::DumpLoadedAssets)
);

USuperAssetManager::USuperAssetManager()
{

}

USuperAssetManager& USuperAssetManager::Get()
{
	check(GEngine);

	if (USuperAssetManager* Singleton = Cast<USuperAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(Project, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to USuperAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<USuperAssetManager>();
}

void USuperAssetManager::DumpLoadedAssets()
{
	UE_LOG(Project, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(Project, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(Project, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(Project, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

UObject* USuperAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool USuperAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void USuperAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void USuperAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("USuperAssetManager::StartInitialLoading");

	// This does all of the scanning, need to do this now even if loads are deferred
	Super::StartInitialLoading();
}

UPrimaryDataAsset* USuperAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass,
	const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("SProjectEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(Project, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(Project, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}
