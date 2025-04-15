
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "GUISettings.generated.h"

class UGUIPolicy;

UCLASS(Config = Game, DefaultConfig)
class GGAMECORE_API UGUISettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UGUISettings();

public:
    UPROPERTY(Config, EditDefaultsOnly, Category = "UI")
    TSoftClassPtr<UGUIPolicy> DefaultPolicyClassPtr;

};
