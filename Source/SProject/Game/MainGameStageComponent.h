
#pragma once

#include "Components/GameStateComponent.h"

#include "MainGameStageComponent.generated.h"

class AWaypointPather;

USTRUCT(BlueprintType)
struct FMainGameStageData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AWaypointPather* WaypointPather = nullptr;

};

UCLASS(Blueprintable, Abstract, HideCategories = (Trigger, PhysicsVolume))
class UMainGameStageComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void RegisterGameStage(int32 GameStageId, FMainGameStageData GameStageData);

    UFUNCTION(BlueprintPure)
    const FMainGameStageData& GetMainGameStageData(int32 GameStageId) const;

public:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TMap<int32, FMainGameStageData> MainGameStageDataMap;
};
