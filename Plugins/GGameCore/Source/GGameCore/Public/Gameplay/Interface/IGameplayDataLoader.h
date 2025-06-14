// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IGameplayDataLoader.generated.h"

struct FGErrorInfo;
class UGameplayDataAsset;

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint), Category = "Gameplay")
class GGAMECORE_API UGameplayDataLoader : public UInterface
{
    GENERATED_BODY()
};

class GGAMECORE_API IGameplayDataLoader : public IInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "GameFeature", meta = (ReturnDisplayName = "Error", WorldContext = "WorldContextObject"))
    virtual FGErrorInfo LoadGameData(const UObject* WorldContextObject, FPrimaryAssetId GameDataAssetId, UPARAM(ref) UGameplayDataAsset*& GameDataAsset);

};
