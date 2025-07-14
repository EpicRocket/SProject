// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Core/Component/GGameCoreComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "StageSpawnComponent.generated.h"

class AStageUnitCharacter;
class AStageAIController;

UCLASS(Abstract, Blueprintable, meta = (BlueprintSpawnableComponent), Category = "Stage", ClassGroup = "Stage")
class MY_API UStageSpawnComponent : public UGGameCoreComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FGErrorInfo BeginSpawn(UClass* UnitClass, FVector Location, FRotator Rotation, TSubclassOf<AStageAIController> AIController, AStageUnitCharacter*& SpawningUnit);

	UFUNCTION(BlueprintCallable)
	FGErrorInfo EndSpawn(AStageUnitCharacter* SpawningUnit, FVector Location, FRotator Rotation);

};
