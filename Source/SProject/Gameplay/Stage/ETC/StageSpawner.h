
#pragma once

#include "GameFramework/Actor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"
#include "Gameplay/ETC/GameplayTeamActor.h"

#include "StageSpawner.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageSpawner : public AGameplayTeamActor, public IGameplayActorTag
{
    GENERATED_BODY()

};
