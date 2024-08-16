
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AssetHelper.generated.h"

class UTexture2D;

UCLASS()
class MY_API UAssetHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Asset Helper", meta = (CompactNodeTitle = "Texture2D"))
    static UTexture2D* GetTexture2DFromSoftObject(TSoftObjectPtr<UTexture2D> AssetPtr);
};
