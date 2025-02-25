
#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ETC/GameplayTeamActor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"

#include "StageStartPoint.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageStartPoint : public AGameplayTeamActor, public IGameplayActorTag
{
	GENERATED_BODY()

public:

};
