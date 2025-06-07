// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "StageMonsterGroupTypes.generated.h"

USTRUCT(BlueprintType)
struct MY_API FStageMonsterGroupDetail
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Sort = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MonsterKind = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct MY_API FStageMonsterGroupInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Group = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStageMonsterGroupDetail> Monsters = {};
};