
#pragma once

#include "Components/GameStateComponent.h"

#include "MainGameStageComponent.generated.h"

class AUnit;
class AWaypointPather;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FMainGameStageData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AWaypointPather* WaypointPather = nullptr;

    bool IsValidData() const;
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

    UFUNCTION(BlueprintCallable)
    void SummonUnit(int32 GameStageId, TSubclassOf<AUnit> UnitClass);

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnSpawnedUnit(AUnit* Unit, UAbilitySystemComponent* AbilitySystem);

public:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TMap<int32, FMainGameStageData> MainGameStageDataMap;
};
