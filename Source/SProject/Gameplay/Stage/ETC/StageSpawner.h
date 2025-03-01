
#pragma once

#include "GameFramework/Actor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"
#include "Gameplay/ETC/GameplayTeamActor.h"
#include "AIController.h"

#include "StageSpawner.generated.h"

class AStageMonsterUnit;
class AStageLevel;
class UBehaviorTree;
struct FGErrorInfo;

USTRUCT(BlueprintType)
struct FStageMonsterSpawnParams
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TWeakObjectPtr<AStageLevel> StageLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PathPosition = INDEX_NONE;

};

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageSpawner : public AGameplayTeamActor, public IGameplayActorTag
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    FGErrorInfo SpawnMonster(const FStageMonsterSpawnParams& Params, AStageMonsterUnit*& SpawnedUnit);
    
protected:
    FGErrorInfo GetSpawnComponent(class UStageSpawnComponent*& SpawnComponent) const;
};
