// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "StageWaveTypes.generated.h"

USTRUCT(BlueprintType)
struct MY_API FStageWaveGroupInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Type = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MonsterGroup = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DelayTime = INDEX_NONE;
	
};
