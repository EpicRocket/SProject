

#pragma once

#include "Components/GameStateComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"

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
	AStageTowerUnit* SpawnTower(uint8 TeamID, AStageLevel* TargetLevel, FVector Location, FRotator Rotation, struct FBuildStageTower Build, TSubclassOf<AAIController> AIController = nullptr);
	
	/*UFUNCTION(BlueprintCallable)
	AStageMonsterUnit* SpawnMonster(FVector Location, FRotator Rotation);*/
	
};

UCLASS()
class MY_API UStageSpawnHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "스테이지|생성")
	static bool SpawnTower(uint8 TeamID, AStageLevel* TargetLevel, FVector Location, FRotator Rotation, struct FBuildStageTower Build, TSubclassOf<AAIController> AIController, AStageTowerUnit*& SpawnedUnit);

};
