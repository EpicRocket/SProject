// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Core/Component/GGameCoreComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "StageSpawnComponent.generated.h"

class AAIController;
class AStageLevel;
struct FLatentActionInfo;

class AStageTowerUnit;
struct FStageTowerInfo;

class AStageMonsterUnit;
struct FStageMonsterInfo;

class AStageUnitCharacter;

UCLASS(Abstract, Blueprintable, meta = (BlueprintSpawnableComponent), Category = "Stage", ClassGroup = "Stage")
class MY_API UStageSpawnComponent : public UGGameCoreComponent
{
	GENERATED_BODY()

public:
	/*UFUNCTION(BlueprintCallable)
	FGErrorInfo BeginSpawn();

	UFUNCTION(BlueprintCallable)
	FGErrorInfo EndSpawn();*/

	UFUNCTION(BlueprintCallable)
	FGErrorInfo SpawnTower(uint8 TeamID, AStageLevel* StageLevel, FVector Location, FRotator Rotation, FStageTowerInfo TowerInfo, TSubclassOf<AAIController> AIController, AStageTowerUnit*& SpawnedUnit);
	
	UFUNCTION(BlueprintCallable)
	FGErrorInfo SpawnMonster(uint8 TeamID, AStageLevel* StageLevel, FVector Location, FRotator Rotation, FStageMonsterInfo MonsterInfo, TSubclassOf<AAIController> AIController, AStageMonsterUnit*& SpawnedUnit);
	
};

UCLASS()
class MY_API UStageSpawnHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FGErrorInfo SpawnTower(uint8 TeamID, AStageLevel* StageLevel, FVector Location, FRotator Rotation, FStageTowerInfo TowerInfo, TSubclassOf<AAIController> AIController, AStageTowerUnit*& SpawnedUnit);

	UFUNCTION(BlueprintCallable)
	static FGErrorInfo SpawnMonster(uint8 TeamID, AStageLevel* StageLevel, FVector Location, FRotator Rotation, FStageMonsterInfo MonsterInfo, TSubclassOf<AAIController> AIController, AStageMonsterUnit*& SpawnedUnit);

};
