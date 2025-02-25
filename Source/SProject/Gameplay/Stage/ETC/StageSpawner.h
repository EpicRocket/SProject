
#pragma once

#include "GameFramework/Actor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"
#include "Gameplay/ETC/GameplayTeamActor.h"

#include "StageSpawner.generated.h"

class AUnitCharacter;
class AStageLevel;

USTRUCT(BlueprintType)
struct FStageSpawnParams
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AStageLevel> StageLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SpawnIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PathPosition = INDEX_NONE;
};

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageSpawner : public AGameplayTeamActor, public IGameplayActorTag
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual AUnitCharacter* Spawn(const FStageSpawnParams& Params);
    
};
