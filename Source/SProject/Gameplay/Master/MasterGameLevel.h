
#pragma once

#include "Gameplay/MyGameLevel.h"

#include "MasterGameLevel.generated.h"

namespace EEndPlayReason {
    enum Type : int;
}

UCLASS()
class MY_API AMasterGameLevel : public AMyGameLevel, public IGLoadingProcessInterface
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // IGLoadingProcessInterface
    virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
    // ~IGLoadingProcessInterface
};
