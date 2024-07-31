
#pragma once

#include "UI/GUIManagerSubsystem.h"

#include "MyUISubsystem.generated.h"

class UMyUIPolicy;
class UMyLocalPlayer;

UCLASS(Config = SProject)
class MY_API UMyUISubsystem : public UGUIManagerSubsystem
{
    GENERATED_BODY()

public:
    static UMyUISubsystem* Get(const UObject* WorldContextObject);
};
