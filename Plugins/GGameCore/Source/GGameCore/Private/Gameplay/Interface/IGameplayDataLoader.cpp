// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Gameplay/Interface/IGameplayDataLoader.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction.h"
// include GameCore
#include "Error/GError.h"
#include "Gameplay/GameplayDataAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IGameplayDataLoader)

const UGameplayDataAsset* IGameplayDataLoader::LoadGameData(const UObject* WorldContextObject, FPrimaryAssetId GameDataAssetId, FGErrorInfo& Error)
{
    check(GEngine);

    auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        Error = GameCore::Throw(GameErr::WORLD_INVALID, TEXT("WorldContextObject isn't valid"));
        return nullptr;
    }

    TObjectPtr<UAssetManager> AssetManager = GEngine->AssetManager;
    if (!IsValid(AssetManager))
    {
        Error = GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("AssetManager isn't found"));
        return nullptr;
    }

    auto AssetPath = AssetManager->GetPrimaryAssetPath(GameDataAssetId);
    auto AssetClass = AssetPath.TryLoad();
    if (!IsValid(AssetClass))
    {
        Error = GameCore::Throw(GameErr::OBJECT_INVALID, FString::Printf(TEXT("%s load failure"), *GameDataAssetId.ToString()));
        return nullptr;
    }

    TSubclassOf<UGameplayDataAsset> GameDataAssetClass = Cast<UClass>(AssetClass);
    if (!IsValid(GameDataAssetClass))
    {
        Error = GameCore::Throw(GameErr::OBJECT_INVALID, FString::Printf(TEXT("%s cast failure"), *GameDataAssetId.ToString()));
        return nullptr;
    }
    auto GameDataAsset = GetDefault<UGameplayDataAsset>(GameDataAssetClass);

    FGameFeatureActivatingContext Context;
    const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(World);
    if (ExistingWorldContext)
    {
        Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
    }

    for (UGameFeatureAction *Action : GameDataAsset->Actions)
    {
        if (Action)
        {
            Action->OnGameFeatureRegistering();
            Action->OnGameFeatureLoading();
            Action->OnGameFeatureActivating(Context);
        }
    }

    Error = GameCore::Pass();
    return GameDataAsset;
}
