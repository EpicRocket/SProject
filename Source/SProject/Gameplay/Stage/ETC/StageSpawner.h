
#pragma once

#include "GameFramework/Actor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"
#include "Gameplay/ETC/GameplayTeamActor.h"

#include "StageSpawner.generated.h"

class AUnitCharacter;
class AStageLevel;
class AGameplayPathActor;

USTRUCT(BlueprintType)
struct FStageSpawnParams
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AStageLevel> StageLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AUnitCharacter> SpawnUnit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PathPosition = INDEX_NONE;
};

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageSpawner : public AGameplayTeamActor, public IGameplayActorTag
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    AUnitCharacter* Spawn(const FStageSpawnParams& Params);
    
    UPROPERTY(EditAnywhere)
    TArray<TSubclassOf<AUnitCharacter>> SpawnUnits;

    UFUNCTION(BlueprintCallable)
    int32 GetCurrentWave();

protected:

private:
    int32 CurrentWave;

    UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
    AGameplayPathActor* GameplayPathActor;
};
