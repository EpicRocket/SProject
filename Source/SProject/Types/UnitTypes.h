
#pragma once

#include "CoreMinimal.h"

#include "UnitTypes.generated.h"

USTRUCT(BlueprintType)
struct MY_API FUnit{
	GENERATED_BODY()};

USTRUCT(BlueprintType)
struct MY_API FTower
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Key = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Position = INDEX_NONE;

	bool operator==(const FTower& Other) const
	{
		return Tie(Key, Level, Position) == Tie(Other.Key, Other.Level, Other.Position);
	}

	bool operator<(const FTower& Other) const
	{
		return Tie(Key, Level, Position) < Tie(Other.Key, Other.Level, Other.Position);
	}
};
