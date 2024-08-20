
#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "StageWorldSubsystem.generated.h"

class AStageLevel;

UCLASS()
class MY_API UStageWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "스테이지")
    void AddLevel(int32 StageKey, AStageLevel* Level);

    UFUNCTION(BlueprintCallable, Category = "스테이지")
    void RemoevLevel(int32 StageKey);

public:
    TMap<int32, TWeakObjectPtr<AStageLevel>> ActivatableLevels;
};
