
#pragma once

#include "StageUnitCharacter.h"

#include "StageTowerUnit.generated.h"

struct FBuildStageTower;

UCLASS(Abstract)
class MY_API AStageTowerUnit : public AStageUnitCharacter
{
    GENERATED_BODY()

public:
    void SetInfo(FBuildStageTower NewInfo);
    
    UFUNCTION(BlueprintPure)
	FBuildStageTower GetInfo() const;
	TSharedRef<FBuildStageTower> GetInfoRef() const;

protected:
    virtual void InitailizeBaseStats() override;

public:
    TSharedPtr<FBuildStageTower> Info;
};
