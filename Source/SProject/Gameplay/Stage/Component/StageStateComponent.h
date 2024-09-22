
#pragma once

#include "Components/GameStateComponent.h"
#include "Loading/Interface/GLoadingProcessInterface.h"

#include "StageStateComponent.generated.h"

UCLASS()
class MY_API UStageStateComponent : public UGameStateComponent, public IGLoadingProcessInterface
{
    GENERATED_BODY()

public:
    // IGLoadingProcessInterface
    virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
    // ~IGLoadingProcessInterface
};
