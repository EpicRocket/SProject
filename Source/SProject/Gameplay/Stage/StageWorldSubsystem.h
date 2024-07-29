
#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "StageWorldSubsystem.generated.h"

class AStageLevel;

UCLASS()
class SPROJECT_API UStageWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:


public:
    UPROPERTY(Transient, BlueprintReadOnly, Category = "스테이지")
    TWeakObjectPtr<AStageLevel> CurrentLevel;
        
    UPROPERTY(Transient)
    TMap<int32, TWeakObjectPtr<AStageLevel>> ActivatableLevels;
};
