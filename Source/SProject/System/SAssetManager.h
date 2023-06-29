
#pragma once

#include "Engine/AssetManager.h"
#include "SAssetManager.generated.h"

/**
 * USAssetManager
 *
 *	Game implementation of the asset manager that overrides functionality and stores game-specific types.
 *	It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic.
 *	This class is used by setting 'AssetManagerClassName' in DefaultEngine.ini.
 */
UCLASS(Config = Game)
class SPROJECT_API USAssetManager : public UAssetManager
{
    GENERATED_BODY()

public:
    USAssetManager();

    static USAssetManager& Get();
    
    // Returns the asset referenced by a TSoftObjectPtr.  This will synchronously load the asset if it's not already loaded.
    template<typename AssetType>
    static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

    // Returns the subclass referenced by a TSoftClassPtr.  This will synchronously load the asset if it's not already loaded.
    template<typename AssetType>
    static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

    // Logs all assets currently loaded and tracked by the asset manager.
    static void DumpLoadedAssets();

protected:
    template <typename GameDataClass>
    const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
    {
        if (TObjectPtr<UPrimaryDataAsset> const * pResult = GameDataMap.Find(GameDataClass::StaticClass()))
        {
            return *CastChecked<GameDataClass>(*pResult);
        }

        // Does a blocking load if needed
        return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
    }
    
    static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
    static bool ShouldLogAssetLoads();

    // Thread safe way of adding a loaded asset to keep in memory.
    void AddLoadedAsset(const UObject* Asset);

    //~UAssetManager interface
    virtual void StartInitialLoading() override;
    //~End of UAssetManager interface
    
    UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);

protected:
    // Loaded version of the game data
    UPROPERTY(Transient)
    TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;

private:
    // Assets loaded and tracked by the asset manager.
    UPROPERTY()
    TSet<TObjectPtr<const UObject>> LoadedAssets;

    // Used for a scope lock when modifying the list of load assets.
    FCriticalSection LoadedAssetsCritical;
};

template <typename AssetType>
AssetType* USAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
    AssetType* LoadedAsset = nullptr;

    const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

    if (AssetPath.IsValid())
    {
        LoadedAsset = AssetPointer.Get();
        if (!LoadedAsset)
        {
            LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
            ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
        }

        if (LoadedAsset && bKeepInMemory)
        {
            // Added to loaded asset list.
            Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
        }
    }

    return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> USAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
    TSubclassOf<AssetType> LoadedSubclass;

    const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

    if (AssetPath.IsValid())
    {
        LoadedSubclass = AssetPointer.Get();
        if (!LoadedSubclass)
        {
            LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
            ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
        }

        if (LoadedSubclass && bKeepInMemory)
        {
            // Added to loaded asset list.
            Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
        }
    }

    return LoadedSubclass;
}


