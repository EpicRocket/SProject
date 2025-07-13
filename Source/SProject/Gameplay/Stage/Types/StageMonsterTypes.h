// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "StageMonsterTypes.generated.h"

enum class EAttackType : uint8;
class UTexture2D;
class UBehaviorTree;
class AStageMonsterUnit;

USTRUCT(BlueprintType)
struct MY_API FStageMonsterInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Grade = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType AttackType = static_cast<EAttackType>(0);

	UPROPERTY()
	TSoftClassPtr<AStageMonsterUnit> UnitClassPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AStageMonsterUnit> UnitClass;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> IconPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY()
	TSoftObjectPtr<UBehaviorTree> AIPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBehaviorTree> AI;

};

UCLASS()
class MY_API UStageMonsterContext : public UGContext
{
	GENERATED_BODY()

public:
	virtual void Load() override;

public:
	UPROPERTY()
	FStageMonsterInfo MonsterInfo;

};
