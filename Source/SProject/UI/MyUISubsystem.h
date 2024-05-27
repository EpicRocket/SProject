
#pragma once

#include "UI/GUIManagerSubsystem.h"

#include "MyUISubsystem.generated.h"

class UMyUIPolicy;
class UMyLocalPlayer;

UCLASS(Abstract, Config = SProject)
class SPROJECT_API UMyUISubsystem : public UGUIManagerSubsystem
{
    GENERATED_BODY()

public:
};
