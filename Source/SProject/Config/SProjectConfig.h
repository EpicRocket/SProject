
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/DeveloperSettings.h"

#include "SProjectConfig.generated.h"

UCLASS(Config = SProject, DefaultConfig, meta = (DisplayName = "Generic"))
class USProjectConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
    virtual FName GetCategoryName() const override;

public:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Test")
	bool bTestMode = false;
};
