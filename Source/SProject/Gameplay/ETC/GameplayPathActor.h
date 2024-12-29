
#pragma once

#include "GameFramework/Actor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"

#include "GameplayPathActor.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AGameplayPathActor : public AActor, public IGameplayActorTag
{
    GENERATED_BODY()

public:

};
