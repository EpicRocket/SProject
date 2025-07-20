
#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ETC/GameplayTeamActor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"

#include "StageStartPoint.generated.h"

class AStageSupervisor;

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageStartPoint : public AGameplayTeamActor, public IGameplayActorTag
{
	GENERATED_BODY()

public:
	void Setup(AStageSupervisor* InSupervisor);

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TWeakObjectPtr<AStageSupervisor> Supervisor;

};
