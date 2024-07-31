
#pragma once

#include "Engine/LevelScriptActor.h"

#include "StageLevel.generated.h"

class AStageBuildZone;
namespace EEndPlayReason {
    enum Type : int;
}

UCLASS()
class MY_API AStageLevel : public ALevelScriptActor
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    UFUNCTION(BlueprintCallable)
	void AddBuildZone(AStageBuildZone* BuildZone);

protected:


public:
    TMap<int32, TWeakObjectPtr<AStageBuildZone>> BuildZones;
};
