
#pragma once

#include "GameFramework/Actor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"
#include "Gameplay/ETC/GameplayTeamActor.h"

#include "StageSpawner.generated.h"

class AStageUnitCharacter;
class AStageLevel;
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

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageSpawner : public AGameplayTeamActor, public IGameplayActorTag
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    FGErrorInfo SpawnMonster(const FStageMonsterSpawnParams& Params, AStageUnitCharacter*& SpawnedUnit);
    
};
