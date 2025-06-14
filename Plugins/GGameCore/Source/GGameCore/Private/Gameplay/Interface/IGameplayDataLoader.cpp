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

FGErrorInfo IGameplayDataLoader::LoadGameData(const UObject* WorldContextObject, FPrimaryAssetId GameDataAssetId, UPARAM(ref) UGameplayDataAsset*& GameDataAsset)
{
    check(GEngine);

    auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
		return GameCore::Throw(GameErr::WORLD_INVALID, TEXT("WorldContextObject isn't valid"));
    }

    TObjectPtr<UAssetManager> AssetManager = GEngine->AssetManager;
    if (!IsValid(AssetManager))
    {
        return GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("AssetManager isn't found"));
    }

    auto AssetPath = AssetManager->GetPrimaryAssetPath(GameDataAssetId);
    auto AssetClass = AssetPath.TryLoad();
    if (!IsValid(AssetClass))
    {
        return GameCore::Throw(GameErr::OBJECT_INVALID, FString::Printf(TEXT("%s load failure"), *GameDataAssetId.ToString()));
    }

    TSubclassOf<UGameplayDataAsset> GameDataAssetClass = Cast<UClass>(AssetClass);
    if (!IsValid(GameDataAssetClass))
    {
        return GameCore::Throw(GameErr::OBJECT_INVALID, FString::Printf(TEXT("%s cast failure"), *GameDataAssetId.ToString()));
    }
    GameDataAsset = const_cast<UGameplayDataAsset*>(GetDefault<UGameplayDataAsset>(GameDataAssetClass));

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

    return GameCore::Pass();
}
