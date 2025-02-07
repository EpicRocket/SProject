

#pragma once

#include "Components/GameStateComponent.h"

#include "StageSpawnComponent.generated.h"

class AStageTowerUnit;
class AStageMonsterUnit;
class AAIController;
class AStageLevel;
struct FBuildStageTower;

UCLASS(Abstract, Blueprintable, meta = (BlueprintSpawnableComponent))
class MY_API UStageSpawnComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AStageTowerUnit* SpawnTower(AStageLevel* TargetLevel, FVector Location, FRotator Rotation, struct FBuildStageTower Build, TSubclassOf<AAIController> AIController = nullptr);
	
	/*UFUNCTION(BlueprintCallable)
	AStageMonsterUnit* SpawnMonster(FVector Location, FRotator Rotation);*/
	
};
