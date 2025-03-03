
#pragma once

#include "StageUnitCharacter.h"

#include "StageMonsterUnit.generated.h"

struct FStageMonsterInfo;

UCLASS(Abstract, Config = Game, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageMonsterUnit : public AStageUnitCharacter
{
    GENERATED_BODY()

protected:
    virtual void InitailizeBaseStats() override;

public:
    void SetInfo(FStageMonsterInfo NewInfo);

    UFUNCTION(BlueprintPure)
    FStageMonsterInfo GetInfo() const;
    TSharedRef<FStageMonsterInfo> GetInfoRef() const;

public:
    TSharedPtr<FStageMonsterInfo> Info;

};
