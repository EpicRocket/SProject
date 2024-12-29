
#include "GameFramework/Actor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"

#include "StageSpawner.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageSpawner : public AActor, public IGameplayActorTag
{
    GENERATED_BODY()

};
