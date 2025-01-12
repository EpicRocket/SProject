
#pragma once

#include "GameFramework/Actor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"
#include "Gameplay/ETC/GameplayTeamActor.h"

#include "StageSpawner.generated.h"

class AUnitCharacter;
class AGameplayPathActor;

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageSpawner : public AGameplayTeamActor, public IGameplayActorTag
{
    GENERATED_BODY()

    // TODO: 함수 호출 하면 스폰 되도록
    // - 1. 이 Spawner의 팀을 따라가도록 즉시 스폰
    // - 2. 스폰이 되자마자 AIController를 빙의
    // - 3. 특정 GameplayPathActor를 찾아야 함 ( 스폰 되자마자 자신의 GameplayPathActor를 찾아 넣어주기)

public:
    UFUNCTION(BlueprintCallable)
    void Spawn();
    
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
