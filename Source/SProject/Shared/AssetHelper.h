
#pragma once

#include "CoreMinimal.h"
#include "AssetHelper.generated.h"

UCLASS(NotBlueprintType)
class UAssetHelper : public UObject
{
    GENERATED_BODY()

public:
	template<typename TAsset>
	static TObjectPtr<TAsset> GetAsset(FSoftObjectPath AssetPath)
	{
		if (AssetPath.IsNull())
		{
			return nullptr;
		}
		return Cast<TAsset>(AssetPath.TryLoad());
	}

	template<typename TClass>
	static UClass* GetClass(FSoftClassPath ClassPath)
	{
		if (ClassPath.IsNull())
		{
			return nullptr;
		}
		return ClassPath.TryLoadClass<TClass>();
	}
};
