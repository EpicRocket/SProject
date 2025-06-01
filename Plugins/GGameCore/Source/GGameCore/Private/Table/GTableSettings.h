
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "GTableSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class GGAMECORE_API UGTableSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGTableSettings();

public:
	UPROPERTY(EditDefaultsOnly, config, Category = "Table")
	FDirectoryPath RootTableDir;

};
