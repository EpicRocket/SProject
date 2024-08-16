
#include "AssetHelper.h"
#include "Engine/Texture2D.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetHelper)

UTexture2D* UAssetHelper::GetTexture2DFromSoftObject(TSoftObjectPtr<UTexture2D> AssetPtr)
{
    if (AssetPtr.IsNull())
    {
        return nullptr;
    }

    if (!AssetPtr.IsValid())
    {
        AssetPtr.LoadSynchronous();
    }

    return AssetPtr.Get();
}
