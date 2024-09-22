
#pragma once

#include "Gameplay/MyGameLevel.h"

#include "MasterGameLevel.generated.h"

UCLASS()
class MY_API AMasterGameLevel : public AMyGameLevel
{
    GENERATED_BODY()

public:
    // IGLoadingProcessInterface
    virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
    // ~IGLoadingProcessInterface
};
