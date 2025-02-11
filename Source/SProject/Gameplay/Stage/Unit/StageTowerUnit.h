
#pragma once

#include "StageUnitCharacter.h"

#include "StageTowerUnit.generated.h"

struct FStageTowerInfo;

UCLASS(Abstract)
class MY_API AStageTowerUnit : public AStageUnitCharacter
{
    GENERATED_BODY()

public:
    void SetInfo(FStageTowerInfo NewInfo);
    
    UFUNCTION(BlueprintPure)
	FStageTowerInfo GetInfo() const;
	TSharedRef<FStageTowerInfo> GetInfoRef() const;

protected:
    virtual void InitailizeBaseStats() override;

public:
    TSharedPtr<FStageTowerInfo> Info;
};
