
#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Templates/SubclassOf.h"

#include "MyAssetManager.generated.h"

UCLASS(Config = SProject)
class MY_API UMyAssetManager : public UAssetManager
{
    GENERATED_BODY()

public:
	virtual void StartInitialLoading() override;


private:
	
};
